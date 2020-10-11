#pragma once

#include <boost/asio/buffer.hpp>
#include "boost/asio/io_context.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "boost/asio/read_until.hpp"
#include "boost/asio/steady_timer.hpp"
#include "boost/asio/write.hpp"
#include <functional>
#include <iostream>
#include <string>
#include <memory>
#include <thread>

using namespace boost::asio;
using ip::tcp;
using std::placeholders::_1;
using std::placeholders::_2;

class client
{
public:
    client(io_context& io_context);
    void start(tcp::resolver::results_type endpoints);
    void stop();
    void start_read();
    void start_write();
    void updateInfo(std::string);
    std::string getInfo();
    std::string getAnswer();
    void clearAnswer();

private:
    void start_connect(tcp::resolver::results_type::iterator endpoint_iter);
    void handle_connect(const std::error_code& error,
        tcp::resolver::results_type::iterator endpoint_iter);
    void handle_read(const std::error_code& error, std::size_t n);
    void handle_write(const std::error_code& error);
    void check_deadline();

private:
    bool stopped_ = false;
    tcp::resolver::results_type endpoints_;
    tcp::socket socket_;
    std::string input_buffer_;
    steady_timer deadline_;
    steady_timer heartbeat_timer_;
    std::string id_;
    std::string info_;
    std::string answer_;
};

class Multiplayer
{
public:
    Multiplayer();
    std::string run(std::string info);

private:
    std::unique_ptr<client> c;
    io_context io;
};