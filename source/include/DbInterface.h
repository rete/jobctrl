/// \file DbInterface.h
/*
 *
 * DbInterface.h header template automatically generated by a class generator
 * Creation date : mar. sept. 27 2016
 *
 * This file is part of procctrl libraries.
 *
 * procctrl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 *
 * procctrl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with procctrl.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Remi Ete
 * @copyright Remi Ete
 */


#ifndef DBINTERFACE_H
#define DBINTERFACE_H

// -- mysql headers
#include <mysql.h>

// -- procctrl headers
#include "ProcCtrlInternal.h"

namespace procctrl {

  /**
   *  @brief  Base64 class
   */
  class Base64Helper
  {
  public:
    /**
     *  @brief  Get the base64 buffer length to allocate given a binary buffer length
     */
    static int getBase64EncodeLength(int binaryLength);

    /**
     *  @brief  Encode the binary buffer into a base64 buffer
     */
    static int toBase64(char * coded_dst, const char *plain_src, int len_plain_src);

    /**
     *  @brief  Get the binary buffer length to allocate given a base64 buffer
     */
    static int getBase64DecodeLength(const char * coded_src);

    /**
     *  @brief  Decode the base64 buffer into a binary buffer
     */
    static int fromBase64(char * plain_dst, const char *coded_src);
  };

  //----------------------------------------------------------------------------------
  //----------------------------------------------------------------------------------

  /**
   *  @brief  DbInterface class
   */
  class DbInterface
  {
  public:
    /**
     *  @brief  Constructor
     */
    DbInterface();

    /**
     *  @brief  Destructor
     */
    virtual ~DbInterface();

    /**
     *  @brief  Connect to mysql database
     */
    void connect(
        const std::string &host,
        const std::string &password
    );

    /**
     *  @brief  Disconnect from the database
     */
    void disconnect();

    /**
     *  @brief  Whether the user is connected to the database
     */
    bool isConnected() const;

    /**
     *  @brief  Get the mysql database host
     */
    const std::string &getHost() const;

    /**
     *  @brief  Check the group password in database
     */
    bool checkGroupPassword(
        const std::string &group,
        const std::string &password
    );

    /**
     *  @brief  Convert string to sha256 key using openssl
     */
    void passwordToSha256(
        const std::string &password,
        unsigned char *sha256
    ) const;

    /**
     *  @brief  Get the maximum number of process that
     *          can be run for a target group
     */
    unsigned int getMaxServerNProcess(
        const std::string &group
    );

    /**
     *  @brief  Create a new group in database
     */
    void createNewGroup(
        const std::string &group,
        const std::string &groupPassword
    );

    /**
     *  @brief  Removed an existing group from database
     */
    void removeGroup(
        const std::string &group,
        const std::string &groupPassword
    );

    /**
     *  @brief  Get available groups in database
     */
    void getGroups(
        std::vector<std::string> &groups
    );




  protected:
    /**
     *  @brief  Execute query to database
     */
    void execute(
        const std::string &query
    );

    /**
     *  @brief  Execute query and handle result in a handler function
     */
    template <typename Handler>
    void query(
        const std::string &query,
        Handler handler
    );

    /**
     *  @brief  Convert input user password to db password format (pwd -> sha256 -> base64)
     */
    void userToDbPassword(
        const std::string &userPassword,
        std::string &dbPassword
    ) const;

    /**
     *  @brief  Whether the two password are equals.
     *          The rawDbPassword must be a password extracted from the db
     *          and user password a user provided password i readable format
     */
    bool samePasswords(
        const std::string &rawDbPassword,
        const std::string &userPassword
    ) const;

  private:
    MYSQL                       *m_pMySQL;       ///< The mysql instance
    std::string                  m_host;         ///< The database host to connect to
    std::string                  m_password;     ///< The database password
    bool                         m_isConnected;  ///< Whether a connection to the database has been created
  };

  //----------------------------------------------------------------------------------
  //----------------------------------------------------------------------------------

  template <typename Handler>
  inline void DbInterface::query(
      const std::string &query,
      Handler handler
  )
  {
    if(!this->isConnected())
      throw Exception(NOT_INITIALIZED, "Database connection not initialized !");

    if(mysql_query(m_pMySQL, query.c_str()))
      throw Exception(FAILURE, std::string("MySQL query failed : ") + mysql_error(m_pMySQL));

    MYSQL_RES *pMySQLResult = mysql_store_result(m_pMySQL);

    if(!pMySQLResult)
      throw Exception(FAILURE, std::string("MySQL store result failed : ") + mysql_error(m_pMySQL));

    handler( pMySQLResult );

    mysql_free_result(pMySQLResult);
  }

}

#endif  //  DBINTERFACE_H
