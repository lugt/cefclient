#include "SeasHandler.h"

#include <sstream>
#include <string>

#include <stdio.h>
#include <stdlib.h>


#include "include/cef_base.h"
#include "include/cef_crash_util.h"
#include "include/cef_dom.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_message_router.h"
#include "tests/cefclient/browser/sqlite3.h"

CefV8HandlerImpl::CefV8HandlerImpl()
{

}


CefV8HandlerImpl::~CefV8HandlerImpl()
{
	// Remove any JavaScript callbacks registered for the context that has been released.
	if (!callback_map_.empty()) {
		CallbackMap::iterator it = callback_map_.begin();
		for (; it != callback_map_.end();) {
			if (it->second.first->IsSame(NULL)) //context))
				callback_map_.erase(it++);
			else
				++it;
		}
	}
}

std::string SeaCefUtils::status;
std::map<std::string, CefRefPtr<CefV8Value>> SeaCefUtils::functionMap;

void SeaCefUtils::setStatus(std::string str) {
	status = str;
}

std::string SeaCefUtils::getStatus() {
	return status;
}

// in CefV8HandlerImpl.cpp
bool CefV8HandlerImpl::Execute(const CefString& name  //JavaScript calling function name(registered)
	, CefRefPtr<CefV8Value> object                 //JavaScript
	, const CefV8ValueList& arguments              // Parameters
	, CefRefPtr<CefV8Value>& retval                // Return Values;
	, CefString& exception)                        //return exep . JavaScript
{
	// In the CefV8Handler::Execute implementation for “registerJavascriptFunction”.
	bool handle = false; // authorize
	if (name == "SEA_seaAuth") {
		if (arguments.size() == 2 && arguments[0]->IsString() && arguments[1]->IsString()) {
			std::string userName = arguments[0]->GetStringValue();
			std::string passWord = arguments[1]->GetStringValue();

			CefRefPtr<CefV8Value> ret = CefV8Value::CreateObject(NULL, NULL);

			if (userName.length() > 0 && passWord.length() > 0) {
				//用户名、密码有效 ，展开登陆逻辑， 操作结束后，返回retval  
				CefRefPtr<CefV8Value> str = CefV8Value::CreateString("1.0.7a");
				ret->SetValue("version", str, V8_PROPERTY_ATTRIBUTE_NONE);
								
				sqlite3 *db;				char *zErrMsg = 0;				int  rc;				char *sql;

				/* Open database */
				rc = sqlite3_open("test.db", &db);
				std::string errmsg;
				if (rc) {
					errmsg  = "Can't open database: \n";
					errmsg += CefString(sqlite3_errmsg(db));
					ret->SetValue("ret", CefV8Value::CreateInt(-920), V8_PROPERTY_ATTRIBUTE_NONE);
				}else {
					errmsg = "Opened database successfully\n";
					/* Create SQL statement */
					sql = "CREATE TABLE COMPANY("  \
						"ID INT PRIMARY KEY     NOT NULL," \
						"NAME           TEXT    NOT NULL," \
						"AGE            INT     NOT NULL," \
						"ADDRESS        CHAR(50)," \
						"SALARY         REAL );";
					ret->SetValue("sql", CefV8Value::CreateString(sql), V8_PROPERTY_ATTRIBUTE_NONE);

					/* Execute SQL statement */
					std::string exec_msg;
					rc = sqlite3_exec(db, sql, SeasResourceHandler::c0allback, 0, &zErrMsg);
					if (rc != SQLITE_OK) {
						exec_msg = "SQL error: %s\n";
						exec_msg += zErrMsg;
						sqlite3_free(zErrMsg);
					}	else {
						exec_msg = "Table created successfully\n";
					}
					sqlite3_close(db);
					ret->SetValue("db_exec_msg", CefV8Value::CreateString(exec_msg), V8_PROPERTY_ATTRIBUTE_NONE);
					ret->SetValue("ret", CefV8Value::CreateInt(1000), V8_PROPERTY_ATTRIBUTE_NONE);
					SeaCefUtils::setStatus("login" + userName); // 标记登陆成功   
				} 
				ret->SetValue("db_open_msg", CefV8Value::CreateString(errmsg), V8_PROPERTY_ATTRIBUTE_NONE);
				retval = ret;
				handle = true;
			}
		} else {
			// Argument Error
			CefRefPtr<CefV8Value> ret = CefV8Value::CreateObject(NULL, NULL);
			//Values are assigned to an array using the SetValue() method variant that takes an index as the first argument.
			// Add two values to the array.
			ret->SetValue("ret", CefV8Value::CreateInt(-1002), V8_PROPERTY_ATTRIBUTE_NONE);
			retval = ret;

			handle = true;
		}
	}else if (name == "SEA_userstatus") {

		CefRefPtr<CefV8Value> ret = CefV8Value::CreateObject(NULL, NULL);
			if (SeaCefUtils::getStatus().substr(0,5) == "login") { // 检查登陆状态   
				//Values are assigned to an array using the SetValue() method variant that takes an index as the first argument.
				// Add two values to the array.
				ret->SetValue("ret", CefV8Value::CreateInt(1000), V8_PROPERTY_ATTRIBUTE_NONE);
			}else {
				//Values are assigned to an array using the SetValue() method variant that takes an index as the first argument.
				// Add two values to the array.
				ret->SetValue("ret", CefV8Value::CreateInt(-1007), V8_PROPERTY_ATTRIBUTE_NONE);
			}
			retval = ret;
	}else if(name == "SEA_onreturn"){
		if (arguments.size() == 2 && arguments[0]->IsString() &&arguments[1]->IsFunction()) { // 相关的name + callback 
			std::string message_name = arguments[0]->GetStringValue();
			CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
			int browser_id = context->GetBrowser()->GetIdentifier();
			SeaCefUtils::registerCallback(browser_id, message_name, arguments[1]);
			// 记录相关的callback
		}
	}else 
	if (name == "registerJavascriptFunction") {
		if (arguments.size() == 2 && arguments[0]->IsString() &&
			arguments[1]->IsFunction()) {
			std::string message_name = arguments[0]->GetStringValue();
			CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
			int browser_id = context->GetBrowser()->GetIdentifier();
			callback_map_.insert(
				std::make_pair(std::make_pair(message_name, browser_id),
					std::make_pair(context, arguments[1])));
			handle = true;
		}

		if (name == "sendMessage") {
			//SendMessage
			handle = true;
		}

		if (!handle) {
			exception = "not implement function";
		}

		return true;
	}
	return true;
}


/**


//std::string sql = "SELECT FROM users WHERE usernmae = '"+userName+"'";
//std::string sql = "SELECT FROM users WHERE usernmae = 'aaa'";
//{equipId:182, group:1, name:"louxiadaxingjiqiyihao", delay:100, period :1000 }
//sqlite_exec_sql(sql);

*/