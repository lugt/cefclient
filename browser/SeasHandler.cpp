#include "SeasHandler.h"

#include <sstream>
#include <string>

#include "include/cef_base.h"
#include "include/cef_crash_util.h"
#include "include/cef_dom.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_message_router.h"

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

// in CefV8HandlerImpl.cpp
bool CefV8HandlerImpl::Execute(const CefString& name  //JavaScript calling function name(registered)
	, CefRefPtr<CefV8Value> object                 //JavaScript
	, const CefV8ValueList& arguments              // Parameters
	, CefRefPtr<CefV8Value>& retval                // Return Values;
	, CefString& exception)                        //return exep . JavaScript
{
	// In the CefV8Handler::Execute implementation for “registerJavascriptFunction”.
	bool handle = false; // authorize
	if (name == "seaAuth") {
		if (arguments.size() == 2 && arguments[0]->IsString() && arguments[1]->IsString()) {
			std::string userName = arguments[0]->GetStringValue();
			std::string passWord = arguments[1]->GetStringValue();
			if (userName.length() > 0 && passWord.length() > 0) {
				//用户名、密码有效
				// Create a new V8 string value. See the "Basic JS Types" section below.
				CefRefPtr<CefV8Value> str = CefV8Value::CreateString("1.0.7a");
				//userName = "aaa";
				//std::string sql = "SELECT FROM users WHERE usernmae = '"+userName+"'";
				//std::string sql = "SELECT FROM users WHERE usernmae = 'aaa'";
				//{equipId:182, group:1, name:"louxiadaxingjiqiyihao", delay:100, period :1000 }
				//sqlite_exec_sql(sql);
				CefRefPtr<CefV8Value> ret = CefV8Value::CreateObject(NULL,NULL);
				ret->SetValue("ret", CefV8Value::CreateInt(1000), V8_PROPERTY_ATTRIBUTE_NONE);
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
