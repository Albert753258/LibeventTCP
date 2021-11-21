#include <string>
#include <mysql/mysql.h>
#include <vector>

using namespace std;
class SQLUtils{
    public:
        MYSQL *connection, mysql;
        
        SQLUtils(std::string hostname, std::string userName, std::string password, std::string dataBaseName, int port){
            mysql_init(&mysql);
            connection = mysql_real_connect(&mysql, hostname.c_str(), userName.c_str(), password.c_str(), dataBaseName.c_str(), port, 0, 0);
        }
        
        std::string sendSQL(std::string sqlStr){
            MYSQL_ROW row;
            MYSQL_RES *res;
            mysql_query(connection, sqlStr.c_str());
            res = mysql_store_result(&mysql);
            std::string tmp = "";
            if(res != NULL){
                while((row = mysql_fetch_row(res)) != NULL){
                    for (int i = 0 ; i < mysql_num_fields(res); i++){
                        tmp += row[i];
                        tmp += " ";
                    }
                    tmp += "\n";
                }
            }
            return tmp;
        }
};
