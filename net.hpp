#ifndef NET_HPP
#define NET_HPP

/**
 * @brief Amminadab - "The son of Ram is Amminadab."
 *
 * @file net.hpp
 * @author  Norbert Bátfai <nbatfai@gmail.com>
 * @version 0.0.1
 *
 * @section LICENSE
 *
 * Copyright (C) 2015 Norbert Bátfai, batfai.norbert@inf.unideb.hu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @section DESCRIPTION
 *
 * JACOB, https://github.com/nbatfai/jacob
 *
 * "The son of Isaac is Jacob." The project called Jacob is an experiment
 * to replace Isaac's (GUI based) visual imagination with a character console.
 *
 * ISAAC, https://github.com/nbatfai/isaac
 *
 * "The son of Samu is Isaac." The project called Isaac is a case study
 * of using deep Q learning with neural networks for predicting the next
 * sentence of a conversation.
 *
 * SAMU, https://github.com/nbatfai/samu
 *
 * The main purpose of this project is to allow the evaluation and
 * verification of the results of the paper entitled "A disembodied
 * developmental robotic agent called Samu Bátfai". It is our hope
 * that Samu will be the ancestor of developmental robotics chatter
 * bots that will be able to chat in natural language like humans do.
 *
 */

#include <cstring>
#include <sstream>

#include <boost/asio.hpp>

class Net
{
public:

  Net()
  {
  }

  ~Net()
  {
  }

  void write_session ( std::string msg )
  {
    char data[4096];

    int length = std::sprintf ( data, "%s", msg.c_str() );
    try
      {
        boost::asio::write ( client_socket_, boost::asio::buffer ( data, length ) );
      }
    catch ( std::exception& e )
      {
        std::cerr << "Ooops: " << e.what() << std::endl;
      }
  }

  void read_session ( void )
  {
    char data[4096];

    try
      {
        for ( ;; )
          {
            boost::system::error_code err;
            size_t length = client_socket_.read_some ( boost::asio::buffer ( data ), err );

            if ( err == boost::asio::error::eof )
              {
                hasSession = false;
                break;
              }
            else if ( err )
              {
                throw boost::system::system_error ( err );
              }

            if ( length-2 )
              {
                std::string s ( data, data+length-2 );
                buf = s;
                already_read_ = false;
              }
          }
      }
    catch ( std::exception& e )
      {
        std::cerr << "Ooooops: " << e.what() << std::endl;
      }
  }

  void start_server ( unsigned short port )
  {
    std::thread t {&Net::server, this, port };
    t.detach();
  }

  void server ( unsigned short port )
  {
    boost::asio::ip::tcp::acceptor acceptor ( io_service_,
        boost::asio::ip::tcp::endpoint ( boost::asio::ip::tcp::v4(), port ) );

    char data[4096];

    for ( ;; )
      {
        boost::asio::ip::tcp::socket socket ( io_service_ );
        acceptor.accept ( socket );

        if ( !hasSession )
          {
            hasSession = true;
            client_socket_ = std::move ( socket );
            std::thread t {&Net::read_session, this };
            t.detach();
          }
        else
          {
            int length = std::sprintf ( data, "Samu is talking with somebody else" );
            try
              {
                buf.clear();
                boost::asio::write ( socket, boost::asio::buffer ( data, length ) );
              }
            catch ( std::exception& e )
              {
                buf.clear();
                std::cerr << "Ooooops: " << e.what() << std::endl;
              }

          }
      }
  }

  void cg_read ( void )
  {
    if ( buf.size() > 0 &&  !already_read_ /*( ch = wgetch ( shell_w ) ) != ERR*/ )
      {
        already_read_ = true;

        std::string ret ( buf );
        throw ret;

      }
  }

  bool has_session ( void ) const
  {
    return hasSession;
  }

private:
  bool hasSession {false};
  bool already_read_ {false};
  boost::asio::io_service io_service_;
  boost::asio::ip::tcp::socket client_socket_ {io_service_};
  std::string buf;
};

#endif
