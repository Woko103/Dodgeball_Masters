#include "multiplayer.h"

client::client(io_context& io_context) : socket_(io_context),
    deadline_(io_context),
    heartbeat_timer_(io_context)
{
}

//Inicia el proceso de conexion
void client::start(tcp::resolver::results_type endpoints)
{
    id_ = "";
    info_ = "";
    // Start the connect actor.
    endpoints_ = endpoints;
    start_connect(endpoints_.begin());

    deadline_.async_wait(std::bind(&client::check_deadline, this));
}

//Lo para todo
void client::stop()
{
    stopped_ = true;
    std::error_code ignored_error;
    socket_.close();
    deadline_.cancel();
    heartbeat_timer_.cancel();
}

void client::start_connect(tcp::resolver::results_type::iterator endpoint_iter)
{
    if (endpoint_iter != endpoints_.end())
    {
        std::cout << "Trying " << endpoint_iter->endpoint() << "...\n";

        //Establece un plazo para la operacion de conexion
        deadline_.expires_after(std::chrono::minutes(10));

        socket_.async_connect(endpoint_iter->endpoint(),
            std::bind(&client::handle_connect,
            this, _1, endpoint_iter));
    }
    else
    {
        stop();
    }
}

void client::handle_connect(const std::error_code& error,
    tcp::resolver::results_type::iterator endpoint_iter)
{
    if (stopped_)
        return;

    //async_connect abre el socket, por lo que si no esta abierto es que aun no ha sido llamado
    if (!socket_.is_open())
    {
        std::cout << "Connect timed out: No se encontraron rivales\n";

        answer_ = "No";

        //Prueba con el siguiente endpoint disponible
        start_connect(++endpoint_iter);
    }

    //Comprueba si la operacion de conexion ha fallado
    else if (error)
    {
        std::cout << "Connect error (Servidor no disponible): " << error.message() << "\n";

        answer_ = "Error";
        //Debemos cerrar el socket antes de abrir otro
        socket_.close();

        //Prueba con el siguiente endpoint disponible
        start_connect(++endpoint_iter);
    }

    //Si todo va bien
    else
    {
        std::cout << "Connected to " << endpoint_iter->endpoint() << "\n";

        //Input
        start_read();

        //Output
        start_write();
    }
}

void client::start_read()
{
    async_read_until(socket_,
        dynamic_buffer(input_buffer_), '\n',
        std::bind(&client::handle_read, this, _1, _2));
}

void client::handle_read(const std::error_code& error, std::size_t n)
{
    if (stopped_)
        return;

    if (!error)
    {
        //Se extrae el mensaje del buffer sin el salto de linea
        std::string line(input_buffer_.substr(0, n - 1));
        input_buffer_.erase(0, n);

        //Se ignoran los heartbeats
        if (!line.empty())
        {
            //std::cout << "Received: " << line << "\n";
            //Le asignamos un id al cliente si aun no tiene
            if(id_ == "" && line.size() == 3)
            {
                id_ = line;
            }
            
            answer_ = line;
        }

        start_read();
    }
    else
    {
        std::cout << "Error on receive:  " << error.message() << "\n";

        stop();
    }
}

void client::start_write()
{
    if (stopped_)
        return;
    //Ajustamos la informacion que vamos a mandar al servidor
    std::string data;
    if(id_ != "")
    {
        data = id_ + "," + info_;
        info_ = "";
    }
    else
    {
        data = "7Nuevo"; //Mensaje especial para que el server sepa que no tenemos id y nos asigne una
    }

    if (data.size() > 4)//Ojo
    {
        async_write(socket_, buffer(data + "\n", data.size()+1),
            std::bind(&client::handle_write, this, _1));
    }
    else 
    {
        async_write(socket_, buffer("\n", 1),
            std::bind(&client::handle_write, this, _1));
    }
}

void client::handle_write(const std::error_code& error)
{
    if (stopped_)
        return;

    if (!error)
    {
        //Espera antes de mandar un heartbeat
        //heartbeat_timer_.expires_after(std::chrono::milliseconds(1));
        heartbeat_timer_.async_wait(std::bind(&client::start_write, this));

        return;
    }
    else
    {
        std::cout << "Error on heartbeat: " << error.message() << "\n";

        stop();
    }
}

void client::check_deadline()
{
    if (stopped_)
        return;

    //Comprueba si el tiempo de espera ha expirado
    if (deadline_.expiry() <= steady_timer::clock_type::now())
    {
        socket_.close();

        deadline_.expires_at(steady_timer::time_point::max());
    }

    //Lo pone a dormir
    deadline_.async_wait(std::bind(&client::check_deadline, this));
}

void client::updateInfo(std::string info)
{
    info_ = info;
}

std::string client::getInfo()
{
    return info_;
}

std::string client::getAnswer()
{
    return answer_;
}

void client::clearAnswer()
{
    answer_ = "";
}

//--------------------------------MULTIPLAYER METHODS---------------------------------------------

Multiplayer::Multiplayer()
{
    try
    {
        tcp::resolver r(io); //Convierte el nombre del servidor especificado como parametro en un endpoint TCP
        
        c = std::make_unique<client>(io);
        c->start(r.resolve("84.124.191.110", "12345")); //IP y puerto. Se pueden cambiar para acceder desde otro lugar
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}

std::string Multiplayer::run(std::string info)
{
    c->updateInfo(info);

    uint16_t i = 0;

    if (info == "")
    {
        while ((c->getAnswer() == "" && i < 150000))
        {
            io.run_one();
            io.restart();

            ++i;
        }
    }
    else
    {
        while ((c->getAnswer() == "" && i < 600) || c->getInfo() != "")
        {
            io.run_one();
            io.restart();

            ++i;
        }
    }
    
    std::string answer = c->getAnswer();
    c->clearAnswer();
        
    return answer;
}