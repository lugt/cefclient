#include "SeasHandler.h"

#include <sstream>
#include <string>

#include <stdio.h>
#include <stdlib.h>

#include <vector>

#include "include/cef_urlrequest.h"
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


int SeaCefUtils::URLRequest(CefRefPtr<CefV8Value> json)
{
	// 将数据上传到服务器
	CefRefPtr<CefRequest> request = CefRequest::Create();
	if (!json->HasValue("url")) {
		return;
	}
	// Set the request method. Supported methods include GET, POST, HEAD, DELETE and PUT.
	std::string met = "GET";
	if (json->HasValue("method")) {
		met = json->GetValue("method")->GetStringValue();
	}
	if (met == "POST") {
		request->SetMethod("POST");
	}



}

void V8toMap(CefRefPtr<CefV8Value> headers, std::map<std::string, std::string> & map) {

	std::vector<CefString> keys;
	//if (!headers->IsArray()) return -1001;
	headers->GetKeys(keys);
	int inner_j, len = keys.size();
	for (inner_j = 0; inner_j < len; inner_j++) {
		map.insert(std::make_pair(keys.at(inner_j), headers->GetValue(keys.at(inner_j))->GetStringValue()));
	}

}


int SeaCefUtils::FileRequest(CefRefPtr<CefV8Value> json)
{
	
	// 将数据上传到服务器
	CefRefPtr<CefRequest> request = CefRequest::Create();

	if (!json->HasValue("url")) {
		return;
	}
	std::string thisurl = json->GetValue("url")->GetStringValue();// FIXME：从参数中取
	request->SetURL(thisurl);
	CefRefPtr<CefPostData> postData = CefPostData::Create();
	request->SetMethod("POST");
	CefRequest::HeaderMap headerMap;
	if (json != NULL && json->HasValue("headers")) {// 从一个JSON参数中取出其他需要传递的参数
		CefRefPtr<CefV8Value> headers = json->GetValue("headers");
		std::vector<CefString> keys;
		//if (!headers->IsArray()) return -1001;
		headers->GetKeys(keys);
		int inner_j, len = keys.size();
		for(inner_j = 0; inner_j < len; inner_j++){ 
			headerMap.insert(std::make_pair(keys.at(inner_j), headers->GetValue(keys.at(inner_j))->GetStringValue()));
		}
	}

	// 将文件列表返回到前端
	time_t time0; 	time(&time0); 	srand(time0);
	std::string boundaryId = "ce-391882-38902-"+ SeaCefUtils::int2str(time0) + "-"+ SeaCefUtils::int2str(rand()) + "-" + SeaCefUtils::int2str(rand());
	std::string bound = "multipart/form-data;boundary="; bound += boundaryId;
	headerMap.insert(std::make_pair("Content-Type", bound));
	// 读取文件并上传

	std::stringstream upload_data_s;

	if (json != NULL && json->HasValue("params")) {// 将所有附加参数写入到form中
		CefRefPtr<CefV8Value> params = json->GetValue("params");
		int len = params->GetArrayLength, inner_i = 0;
		for (inner_i = 0; inner_i < len; inner_i ++) {
			CefString key = params->GetValue(inner_i)->GetStringValue();
			upload_data_s << "--" << boundaryId
				<< "\r\n"
				<< "Content-Disposition: form-data; name=\"" << key.c_str()
				<< "\r\n";
		}
	}

	CefString file = std::wstring(json->GetValue("filename")->GetStringValue());// szFile为待上传的文件
	try {

		std::string fn = file.ToString();
		std::ifstream in(file.ToWString(), std::ios::binary/*需要指定为二进制模式打开*/);
		upload_data_s
			<< "--" << boundaryId
			<< "\r\n"
			<< "Content-Disposition: form-data; name=\"up"; //<< //SetString(count, file);

		// 上传数据的结尾标志
		upload_data_s << "\r\n" << "\r\n" << "--" << boundaryId << "--" << "\r\n";
		const std::string upload_data = upload_data_s.str();
		CefRefPtr<CefPostDataElement> element = CefPostDataElement::Create();

		element->SetToBytes(upload_data.size(), upload_data.c_str());// 最大缺点是文件太大后，传递不过去
		std::stringstream sizeSS; sizeSS << upload_data.size();
		headerMap.insert(std::make_pair("Content-Length", sizeSS.str()));// Content-Length可以不指定
		postData->AddElement(element);
		request->SetHeaderMap(headerMap);
		request->SetPostData(postData);// 发送请求到服务端
		//CefRefPtr<CefBrowser> browser = getBrowser();// 得到当前浏览器
		//CefRefPtr<CefWebURLRequest> mUrlRequest = CefWebURLRequest::Create(request, new RequestClient(), browser->GetHost()->GetRequestContext());
	}catch (std::exception e) {

	}
}

std::string SeaCefUtils::int2str(int num) {
	if (num == 0)
		return "0";

	std::string str = "";

	int num_ = num > 0 ? num : -1 * num;

	while (num_)
	{
		str = (char)(num_ % 10 + 48) + str;
		num_ /= 10;
	}

	if (num < 0)
		str = "-" + str;

	return str;
}
/**


//std::string sql = "SELECT FROM users WHERE usernmae = '"+userName+"'";
//std::string sql = "SELECT FROM users WHERE usernmae = 'aaa'";
//{equipId:182, group:1, name:"louxiadaxingjiqiyihao", delay:100, period :1000 }
//sqlite_exec_sql(sql);

*/