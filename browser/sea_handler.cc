#include "sea_handler.h"

#include <sstream>
#include <string>

#include "include/cef_crash_util.h"
#include "include/cef_dom.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_message_router.h"

bool SeaHandler::Execute(const CefString& name,
		CefRefPtr<CefV8Value> object,
		const CefV8ValueList& arguments,
		CefRefPtr<CefV8Value>& retval,
		CefString& exception) {
		if (name == "register") {
			if (arguments.size() == 1 && arguments[0]->IsFunction()) {
				//callback_func_ = arguments[0];
				//callback_context_ = CefV8Context::GetCurrentContext();
				return true;
			}
		}

		return false;
}

SeaHandler::SeaHandler()
{

}

SeaHandler::~SeaHandler()
{
	// Remove any JavaScript callbacks registered for the context that has been released.
	if (!callback_map_.empty()) {
		CallbackMap::iterator it = callback_map_.begin();
		for (; it != callback_map_.end();) {
			if (it->second.first->IsSame(NULL)) // content))
				callback_map_.erase(it++);
			else
				++it;
		}
	}
}
