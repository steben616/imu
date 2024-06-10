#ifndef WSS_H
#define WSS_H

#include <set>
#include <iostream>

#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

using websocketpp::lib::bind;
using websocketpp::connection_hdl;

struct wss_server {

    typedef websocketpp::server<websocketpp::config::asio> server;
    typedef std::set<connection_hdl, std::owner_less<connection_hdl>> con_list;

    server _server;
    con_list m_connections;

    wss_server(){
        // set up access channels to only log interesting things
        _server.clear_access_channels(websocketpp::log::alevel::all);
        _server.set_access_channels(websocketpp::log::alevel::access_core);
        _server.set_access_channels(websocketpp::log::alevel::app);
        _server.init_asio();
        using websocketpp::lib::placeholders::_1;
        using websocketpp::lib::bind;
        _server.set_open_handler(bind(&wss_server::on_open, this, _1));
        _server.set_close_handler(bind(&wss_server::on_close, this, _1));
    }

    void on_open(connection_hdl hdl) {
        std::cout << "on_open" << std::endl;
        m_connections.insert(hdl);
    }

    void on_close(connection_hdl hdl) {
        std::cout << "on_close" << std::endl;
        m_connections.erase(hdl);
    }

    void send_data(const std::string& msg) {
        con_list::iterator it;
        for (it = m_connections.begin(); it != m_connections.end(); ++it) {
            _server.send(*it, msg, websocketpp::frame::opcode::text);
        }
    }

    void run_server() {
        try {
            _server.listen(9002);
            _server.start_accept();
            _server.run();
        } catch (websocketpp::exception const & e) {
            std::cout << e.what() << std::endl;
        } catch (...) {
            std::cout << "exception!!!" << std::endl;
        }
    }
};

#endif