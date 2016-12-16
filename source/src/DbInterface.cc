/// \file DbInterface.cc
/*
 *
 * DbInterface.cc source template automatically generated by a class generator
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

// -- procctrl headers
#include "DbInterface.h"

// -- openssl headers
#include <openssl/sha.h>

namespace procctrl {


  /* aaaack but it's fast and const should make it shared text page. */
  static const unsigned char pr2six[256] =
      {
          /* ASCII table */
          64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
          64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
          64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
          52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
          64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
          15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
          64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
          41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
          64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
          64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
          64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
          64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
          64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
          64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
          64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
          64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
      };

  //-------------------------------------------------------------------------------------------------

  int Base64Helper::getBase64DecodeLength(const char *bufcoded)
  {
    int nbytesdecoded;
    const unsigned char *bufin;
    int nprbytes;

    bufin = (const unsigned char *) bufcoded;
    while (pr2six[*(bufin++)] <= 63);

    nprbytes = (bufin - (const unsigned char *) bufcoded) - 1;
    nbytesdecoded = ((nprbytes + 3) / 4) * 3;

    return nbytesdecoded + 1;
  }

  //-------------------------------------------------------------------------------------------------

  int Base64Helper::fromBase64(char *bufplain, const char *bufcoded)
  {
    int nbytesdecoded;
    const unsigned char *bufin;
    unsigned char *bufout;
    int nprbytes;

    bufin = (const unsigned char *) bufcoded;
    while (pr2six[*(bufin++)] <= 63);
    nprbytes = (bufin - (const unsigned char *) bufcoded) - 1;
    nbytesdecoded = ((nprbytes + 3) / 4) * 3;

    bufout = (unsigned char *) bufplain;
    bufin = (const unsigned char *) bufcoded;

    while (nprbytes > 4) {
      *(bufout++) =
          (unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
      *(bufout++) =
          (unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
      *(bufout++) =
          (unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
      bufin += 4;
      nprbytes -= 4;
    }

    /* Note: (nprbytes == 1) would be an error, so just ingore that case */
    if (nprbytes > 1) {
      *(bufout++) =
          (unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
    }
    if (nprbytes > 2) {
      *(bufout++) =
          (unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
    }
    if (nprbytes > 3) {
      *(bufout++) =
          (unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
    }

    *(bufout++) = '\0';
    nbytesdecoded -= (4 - nprbytes) & 3;
    return nbytesdecoded;
  }

  //-------------------------------------------------------------------------------------------------

  static const char basis_64[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  //-------------------------------------------------------------------------------------------------

  int Base64Helper::getBase64EncodeLength(int len)
  {
    return ((len + 2) / 3 * 4) + 1;
  }

  //-------------------------------------------------------------------------------------------------

  int Base64Helper::toBase64(char *encoded, const char *string, int len)
  {
    int i;
    char *p;

    p = encoded;
    for (i = 0; i < len - 2; i += 3) {
      *p++ = basis_64[(string[i] >> 2) & 0x3F];
      *p++ = basis_64[((string[i] & 0x3) << 4) |
                      ((int) (string[i + 1] & 0xF0) >> 4)];
      *p++ = basis_64[((string[i + 1] & 0xF) << 2) |
                      ((int) (string[i + 2] & 0xC0) >> 6)];
      *p++ = basis_64[string[i + 2] & 0x3F];
    }
    if (i < len) {
      *p++ = basis_64[(string[i] >> 2) & 0x3F];
      if (i == (len - 1)) {
        *p++ = basis_64[((string[i] & 0x3) << 4)];
        *p++ = '=';
      }
      else {
        *p++ = basis_64[((string[i] & 0x3) << 4) |
                        ((int) (string[i + 1] & 0xF0) >> 4)];
        *p++ = basis_64[((string[i + 1] & 0xF) << 2)];
      }
      *p++ = '=';
    }

    *p++ = '\0';
    return p - encoded;
  }


  DbInterface::DbInterface() :
          m_pMySQL(NULL),
          m_isConnected(false)
  {
    /* nop */
  }

  //----------------------------------------------------------------------------------
  //----------------------------------------------------------------------------------

  DbInterface::~DbInterface()
  {
    if(this->isConnected())
      this->disconnect();
  }

  //----------------------------------------------------------------------------------

  void DbInterface::connect(
      const std::string &host,
      const std::string &password
  )
  {
    if(this->isConnected())
      throw Exception(ALREADY_CONNECTED, "Already connected to database!");

    m_host = host;
    m_password = password;

    try
    {
      // create mysql instance
      m_pMySQL = mysql_init(NULL);

      if(!m_pMySQL)
      {
        std::stringstream errorMessage;
        errorMessage << "Couldn't create mysql instance : " << mysql_error(m_pMySQL);
        throw Exception(FAILURE, errorMessage.str());
      }

      // create connection to database
      if(NULL == mysql_real_connect(m_pMySQL, m_host.c_str(), ProcCtrl::DB_USER, m_password.c_str(),
          NULL, 0, NULL, 0))
      {
        std::stringstream errorMessage;
        errorMessage << "Couldn't initialize mysql connection : " << mysql_error(m_pMySQL);

        throw std::runtime_error(errorMessage.str());
      }

      m_isConnected = true;

      std::stringstream query;
      query << "USE " << ProcCtrl::DB_NAME << ";";

      this->execute(query.str());
    }
    catch(const Exception &exception)
    {
      if(m_pMySQL)
        mysql_close(m_pMySQL);

      m_host = "";
      m_password = "";

      m_isConnected = false;

      throw exception;
    }
    catch(const std::exception &exception)
    {
      if(m_pMySQL)
        mysql_close(m_pMySQL);

      m_host = "";
      m_password = "";

      m_isConnected = false;

      throw Exception(FAILURE, std::string("Caught standard exception while connecting to database : ") + exception.what());
    }
  }

  //----------------------------------------------------------------------------------

  void DbInterface::disconnect()
  {
    if(!this->isConnected())
      return;

    if(m_pMySQL)
      mysql_close(m_pMySQL);
  }

  //----------------------------------------------------------------------------------

  bool DbInterface::isConnected() const
  {
    return m_isConnected;
  }

  //----------------------------------------------------------------------------------

  const std::string &DbInterface::getHost() const
  {
    return m_host;
  }

  //----------------------------------------------------------------------------------

  void DbInterface::execute(
      const std::string &query
  )
  {
    if(!this->isConnected())
      throw Exception(NOT_INITIALIZED, "Database connection not initialized !");

    if(mysql_query(m_pMySQL, query.c_str()))
      throw Exception(FAILURE, std::string("MySQL query failed : ") + mysql_error(m_pMySQL));
  }

  //----------------------------------------------------------------------------------

  bool DbInterface::checkGroupPassword(
      const std::string &group,
      const std::string &password
  )
  {
    std::stringstream query;
    query << "SELECT password FROM GROUPS WHERE name=\"" << group << "\";";

    bool validPassword(false);

    try
    {
      this->query(query.str(),
          [&group, &password, &validPassword, this](MYSQL_RES *pMySQLResult) {

        int nRows = mysql_num_rows(pMySQLResult);

        if(nRows == 0)
          throw Exception(FAILURE, "No group called '" + group + "' in database");

        MYSQL_ROW row = mysql_fetch_row(pMySQLResult);

        std::string pwdString = row[0];
        unsigned char testSha256[SHA256_DIGEST_LENGTH];

        this->passwordToSha256(password, testSha256);

        std::string pwdSha256Base64;
        pwdSha256Base64.resize(Base64Helper::getBase64EncodeLength(SHA256_DIGEST_LENGTH));
        Base64Helper::toBase64(const_cast<char*>(pwdSha256Base64.c_str()), (char*)testSha256, SHA256_DIGEST_LENGTH);
        pwdSha256Base64.resize(pwdSha256Base64.size()-1); // remove last null '\0' character

        validPassword = (pwdSha256Base64 == pwdString);
      });

      return validPassword;
    }
    catch(...)
    {
      return false;
    }
  }

  //----------------------------------------------------------------------------------

  void DbInterface::passwordToSha256(
      const std::string &password,
      unsigned char *sha256
  ) const
  {
    unsigned char *pwd(static_cast<unsigned char *>((void *)password.c_str()));
    unsigned long passwordLength(password.size());

    SHA256_CTX context;

    if(!SHA256_Init(&context))
      throw Exception(FAILURE, "Couldn't initialized SHA256_CTX context");

    if(!SHA256_Update(&context, pwd, passwordLength))
      throw Exception(FAILURE, "Couldn't update SHA256_CTX context");

    if(!SHA256_Final(sha256, &context))
      throw Exception(FAILURE, "Couldn't finalized SHA256_CTX context");
  }

  //----------------------------------------------------------------------------------

  unsigned int DbInterface::getMaxServerNProcess(
      const std::string &group
  )
  {
    std::stringstream query;
    query << "SELECT max_proc FROM GROUPS WHERE name=\"" << group << "\";";

    try
    {
      unsigned int maxProc(0);

      this->query(query.str(),
          [&group, &maxProc, this](MYSQL_RES *pMySQLResult) {

        int nFields = mysql_num_fields(pMySQLResult);

        if(nFields == 0)
          throw Exception(FAILURE, "No group called '" + group + "' in database");

        MYSQL_ROW row = mysql_fetch_row(pMySQLResult);
        maxProc = (unsigned int) row[0];
      }
      );

      return maxProc;
    }
    catch(...)
    {
      return 0;
    }
  }

  //----------------------------------------------------------------------------------

  void DbInterface::createNewGroup(
      const std::string &group,
      const std::string &groupPassword
  )
  {
    {
      std::stringstream query;
      query << "SELECT name FROM GROUPS WHERE name=\"" << group << "\";";

      this->query(query.str(),
          [&group, this](MYSQL_RES *pMySQLResult) {

        int nRows = mysql_num_rows(pMySQLResult);

        if(nRows != 0)
          throw Exception(FAILURE, "Group '" + group + "' already exists in database");
      });
    }

    std::string groupDbPassword;
    userToDbPassword(groupPassword, groupDbPassword);

    std::stringstream query;
    query << "INSERT INTO GROUPS VALUES('" << group << "','" << groupDbPassword << "'," << ProcCtrl::MAX_N_PROCS << ");";
    this->execute(query.str());
  }

  //----------------------------------------------------------------------------------

  void DbInterface::removeGroup(
      const std::string &group,
      const std::string &groupPassword
  )
  {
    if(!this->checkGroupPassword(group, groupPassword))
      throw Exception(INVALID_PASSWORD, "Invalid group password");

    std::stringstream query;
    query << "DELETE FROM GROUPS WHERE name='" << group << "';";
    this->execute(query.str());
  }

  //----------------------------------------------------------------------------------

  void DbInterface::getGroups(
      std::vector<std::string> &groups
  )
  {
    this->query("SELECT name FROM GROUPS;",
        [&groups, this](MYSQL_RES *pMySQLResult) {

          MYSQL_ROW row;

          while((row = mysql_fetch_row(pMySQLResult)))
            groups.push_back(row[0]);
    });
  }

  //----------------------------------------------------------------------------------

  void DbInterface::userToDbPassword(const std::string &userPassword, std::string &dbPassword) const
  {
    // convert user password to sha256
    unsigned char userPasswordSha256[SHA256_DIGEST_LENGTH];
    this->passwordToSha256(userPassword, userPasswordSha256);

    // encode the sha256 string to base 64
    dbPassword.clear();
    dbPassword.resize(Base64Helper::getBase64EncodeLength(SHA256_DIGEST_LENGTH));
    Base64Helper::toBase64(const_cast<char*>(dbPassword.c_str()), (char*)userPasswordSha256, SHA256_DIGEST_LENGTH);
    dbPassword.resize(dbPassword.size()-1); // remove last null '\0' character
  }

  //----------------------------------------------------------------------------------

  bool DbInterface::samePasswords(const std::string &rawDbPassword, const std::string &userPassword) const
  {
    std::string userDbPassword;
    userToDbPassword(userPassword, userDbPassword);

    // compare the db password and user password converted into db format
    return (rawDbPassword == userDbPassword);

  }

}
