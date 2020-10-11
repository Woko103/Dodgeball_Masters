#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include "boost/asio/buffer.hpp"
#include "boost/asio/io_context.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "boost/asio/ip/udp.hpp"
#include "boost/asio/read_until.hpp"
#include "boost/asio/steady_timer.hpp"
#include "boost/asio/write.hpp"

using namespace boost::asio;
using ip::tcp;
using ip::udp;

//---------------------------SUBSCRIBER---------------------------------

class subscriber
{
public:
  virtual ~subscriber() = default;
  virtual void deliver(const std::string& msg) = 0;
  virtual uint16_t getId() = 0;
  virtual uint16_t getGroup() = 0;

private:
  uint16_t id_;
  uint16_t group_;
};

typedef std::shared_ptr<subscriber> subscriber_ptr;

//----------------------------CHANNEL------------------------------------

class channel
{
public:
  void join(subscriber_ptr subscriber)
  {
    subscribers_.insert(subscriber);
  }

  void leave(subscriber_ptr subscriber)
  {
    subscribers_.erase(subscriber);
  }

  void deliver(const std::string& msg)
  {
    //Se extrae el id del mensaje recibido para evitar reenviarlo a quien
    //nos lo ha mandado
    std::string group = msg.substr(0,1);
    std::string id = msg.substr(2,1);

    //std::cout << "GROUP: " << group << std::endl;
    //std::cout << "ID: " << id << std::endl;

    for (const auto& s : subscribers_)
    {
      //std::cout << "group: " << s->getGroup() << std::endl;
      //std::cout << "id: " << s->getId() << std::endl;

      if(s->getGroup() == 0 || group == "7" || (std::to_string(s->getGroup()) == group && std::to_string(s->getId()) != id))
      {
        s->deliver(msg);
      }
    }
  }

  //Asigna un grupo y un id al nuevo jugador
  std::vector<uint16_t> obtainGroupAndId()
  {
    std::vector<uint16_t> groupAndId;
    uint16_t group = 0;
    uint16_t id = 0;

    std::vector<uint16_t> groupN;

    //Creamos 9 grupos
    for (int i=0; i<9; i++)
    {
      groupN.push_back(0);
    }

    //Contamos cuantos jugadores hay por grupo
    for (const auto& s : subscribers_)
    {
      if (s->getGroup() != 0)
      {
        ++groupN[s->getGroup()-1];
      }
    }

    //Si hay hueco en algun grupo, el nuevo jugador se mete ahi
    for (int i=0; i<groupN.size(); i++)
    {
      if (groupN[i] == 1)
      {
        for (const auto& s : subscribers_)
        {
          if (s->getGroup() == groupN[i]+1)
          {
            //Le comunicamos a los otros miembros del grupo el id del nuevo jugador
            s->deliver(std::to_string(i+1) + "," + std::to_string(groupN[i]+1));
          }
        }
        groupAndId.push_back(i+1); //Grupo
        groupAndId.push_back(groupN[i]+1); //Id
        return groupAndId;
      }
    }

    //Si no hay hueco, el jugador se mete en el primer grupo vacio
    for (int i=0; i<groupN.size(); i++)
    {
      if (groupN[i] == 0)
      {
        groupAndId.push_back(i+1); //Grupo
        groupAndId.push_back(1); //Id
        return groupAndId;
      }
    }

    return groupAndId;
  }

private:
  std::set<subscriber_ptr> subscribers_;
};

//----------------------------TCP_SESSION------------------------------------

class tcp_session
  : public subscriber,
    public std::enable_shared_from_this<tcp_session>
{
public:
  tcp_session(tcp::socket socket, channel& ch)
    : channel_(ch),
      socket_(std::move(socket))
  {
    input_deadline_.expires_at(steady_timer::time_point::max());
    output_deadline_.expires_at(steady_timer::time_point::max());

    non_empty_output_queue_.expires_at(steady_timer::time_point::max());
  }

  void start()
  {
    std::vector<uint16_t> groupAndId = channel_.obtainGroupAndId();

    channel_.join(shared_from_this());
    
    group_ = groupAndId[0];
    id_ = groupAndId[1];

    //Le enviamos al cliente el grupo e id que le corresponden
    std::string clientN = std::to_string(group_) + "," + std::to_string(id_) + "\n";
    output_queue_.push_front(clientN);

    read_line();
    check_deadline(input_deadline_);

    await_output();
    check_deadline(output_deadline_);
  }

  uint16_t getId()
  {
    return id_;
  }

  uint16_t getGroup()
  {
    return group_;
  }

private:
  void stop()
  {
    channel_.leave(shared_from_this());

    std::error_code ignored_error;
    socket_.close();
    input_deadline_.cancel();
    non_empty_output_queue_.cancel();
    output_deadline_.cancel();
  }

  bool stopped() const
  {
    return !socket_.is_open();
  }

  void deliver(const std::string& msg) override
  {
    output_queue_.push_back(msg + "\n");

    non_empty_output_queue_.expires_at(steady_timer::time_point::min());
  }

  void read_line()
  {
    //Establece un tiempo para la operacion de lectura
    input_deadline_.expires_after(std::chrono::seconds(150));

    auto self(shared_from_this());
    async_read_until(socket_,
        dynamic_buffer(input_buffer_), '\n',
        [this, self](const std::error_code& error, std::size_t n)
        {
          //Comprueba que la sesion no haya parado
          if (stopped())
            return;

          if (!error)
          {
            //Extrae el mensaje del buffer quitando el salto de linea final
            std::string msg(input_buffer_.substr(0, n - 1));
            input_buffer_.erase(0, n);

            if (!msg.empty())
            {
              channel_.deliver(msg);
            }
            else
            {

              // Si no hay nada que mandar a cliente le mandamos un heartbeat
              if (output_queue_.empty())
              {
                output_queue_.push_back("\n");

                non_empty_output_queue_.expires_at(
                    steady_timer::time_point::min());
              }
            }

            read_line();
          }
          else
          {
            stop();
          }
        });
  }

  void await_output()
  {
    auto self(shared_from_this());
    non_empty_output_queue_.async_wait(
        [this, self](const std::error_code& /*error*/)
        {
          if (stopped())
            return;

          if (output_queue_.empty())
          {
            // Como no hay mensajes que mandar se queda esperando
            non_empty_output_queue_.expires_at(steady_timer::time_point::max());
            await_output();
          }
          else
          {
            write_line();
          }
        });
  }

  void write_line()
  {
    output_deadline_.expires_after(std::chrono::seconds(30));

    auto self(shared_from_this());
    async_write(socket_,
        buffer(output_queue_.front()),
        [this, self](const std::error_code& error, std::size_t /*n*/)
        {
          if (stopped())
            return;

          if (!error)
          {
            output_queue_.pop_front();

            await_output();
          }
          else
          {
            stop();
          }
        });
  }

  void check_deadline(steady_timer& deadline)
  {
    auto self(shared_from_this());
    deadline.async_wait(
        [this, self, &deadline](const std::error_code& /*error*/)
        {
          if (stopped())
            return;

          if (deadline.expiry() <= steady_timer::clock_type::now())
          {
            //Expira la sesion
            stop();
          }
          else
          {
            //Lo pone a dormir
            check_deadline(deadline);
          }
        });
  }

  channel& channel_;
  tcp::socket socket_;
  std::string input_buffer_;
  steady_timer input_deadline_{socket_.get_executor()};
  std::deque<std::string> output_queue_;
  steady_timer non_empty_output_queue_{socket_.get_executor()};
  steady_timer output_deadline_{socket_.get_executor()};
  uint16_t id_;
  uint16_t group_;
};

typedef std::shared_ptr<tcp_session> tcp_session_ptr;

//----------------------------------------------------------------------

class udp_broadcaster
  : public subscriber
{
public:
  udp_broadcaster(io_context& io_context,
      const udp::endpoint& broadcast_endpoint)
    : socket_(io_context)
  {
    socket_.connect(broadcast_endpoint);
    socket_.set_option(udp::socket::broadcast(true));
  }

  uint16_t getId()
  {
    return id_;
  }

  uint16_t getGroup()
  {
    return group_;
  }

private:
  void deliver(const std::string& msg)
  {
    std::error_code ignored_error;
    //std::cout << msg << '\n';
  }
  
  uint16_t id_ = 0;
  uint16_t group_ = 0;

  udp::socket socket_;
};

//----------------------------------------------------------------------

class server
{
public:
  server(io_context& io_context,
      const tcp::endpoint& listen_endpoint,
      const udp::endpoint& broadcast_endpoint)
    : io_context_(io_context), //Union del juego al servicio I/O de boost asio
      acceptor_(io_context, listen_endpoint) //Se encarga de aceptar las conexiones de los clientes
  {
    channel_.join(
        std::make_shared<udp_broadcaster>(
          io_context_, broadcast_endpoint)); //Crea un puntero compartido entre io_context y broadcast_endpoint

    accept();
  }

private:
  void accept()
  {
    //Usado para aceptar de manera asicrona una nueva conexion en un socket
    acceptor_.async_accept(
        [this](const std::error_code& error, tcp::socket socket)
        {
          if (!error)
          {
            std::make_shared<tcp_session>(std::move(socket), channel_)->start();
          }

          accept(); //Se hace un bucle para estar siempre aceptando nuevas conexiones
        });
  }

  io_context& io_context_;
  tcp::acceptor acceptor_;
  channel channel_;
};

//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 4)
    {
      std::cerr << "Usage: server <listen_port> <bcast_address> <bcast_port>\n";
      return 1;
    }

    io_context io_context;

    //Establece el punto de escucha, (puerto), para que el juego reciba los datos
    tcp::endpoint listen_endpoint(tcp::v4(), std::atoi(argv[1]));

    //My IP: 84.124.191.110
    //Local IP: 127.0.0.1

    //Se utiliza para pasar la informacion de un punto al resto de ordenadores en una misma red
    udp::endpoint broadcast_endpoint(
        ip::make_address(argv[2]), std::atoi(argv[3])); //Crea una direccion en formato decimal de un string de una direccion de un IPv4


    server s(io_context, listen_endpoint, broadcast_endpoint);

    io_context.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
