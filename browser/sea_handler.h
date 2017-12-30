#include <sstream>
#include <string>

#include "include/cef_base.h"
#include "include/cef_crash_util.h"
#include "include/cef_dom.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_message_router.h"

	// in CefV8HandlerImpl.h
	class SeaHandler : public CefV8Handler {
	private:
		IMPLEMENT_REFCOUNTING(SeaHandler);  

	public:
		SeaHandler();
		~SeaHandler();
	public:
		/**
		*	CefV8Handler Methods, Which will be called when the V8 extension
		*  is called in the Javascript environment
		*/
		virtual bool Execute(const CefString& name
			, CefRefPtr<CefV8Value> object
			, const CefV8ValueList& arguments
			, CefRefPtr<CefV8Value>& retval
			, CefString& exception);
	private:
		// Map of message callbacks.
		typedef std::map<std::pair<std::string, int>,
			std::pair<CefRefPtr<CefV8Context>, CefRefPtr<CefV8Value> > >
			CallbackMap;
		CallbackMap callback_map_;
	};