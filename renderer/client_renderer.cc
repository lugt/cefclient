// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/cefclient/renderer/client_renderer.h"
#include "tests/cefclient/browser/sea_handler.h"
#include "tests/cefclient/browser/SeasHandler.h"

#include <sstream>
#include <string>

#include "include/cef_crash_util.h"
#include "include/cef_dom.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_message_router.h"


namespace client {
namespace renderer {

namespace {


class MyV8Handler : public CefV8Handler {
public:
	bool Execute(const CefString& name,
		CefRefPtr<CefV8Value> object,
		const CefV8ValueList& arguments,
		CefRefPtr<CefV8Value>& retval,
		CefString& exception) {
		return true;
	}
};

// Must match the value in client_handler.cc.
const char kFocusedNodeChangedMessage[] = "ClientRenderer.FocusedNodeChanged";

class ClientRenderDelegate : public ClientAppRenderer::Delegate {
 public:
  ClientRenderDelegate() : last_node_is_editable_(false) {}

  void OnRenderThreadCreated(CefRefPtr<ClientAppRenderer> app,
                             CefRefPtr<CefListValue> extra_info) OVERRIDE {
    if (CefCrashReportingEnabled()) {
      // Set some crash keys for testing purposes. Keys must be defined in the
      // "crash_reporter.cfg" file. See cef_crash_util.h for details.
      CefSetCrashKeyValue("testkey1", "value1_renderer");
      CefSetCrashKeyValue("testkey2", "value2_renderer");
      CefSetCrashKeyValue("testkey3", "value3_renderer");
    }
  }

  void OnWebKitInitialized(CefRefPtr<ClientAppRenderer> app) OVERRIDE {
    // Create the renderer-side router for query handling.
    CefMessageRouterConfig config;
    message_router_ = CefMessageRouterRendererSide::Create(config);
	/**
	 Extension 可以嵌入Extension代码 (每个页面都会有)
	*/
	std::string extensionCode =
		"var seaExtension;"
		"if (!seaExtension)"
		"  seaExtension = {};"
		"(function() {"
		"  seaExtension.msg = 'Extension Sea is daemoning!';"
		"})();";

	// Register the extension.
	CefRegisterExtension("v8/test", extensionCode, NULL);
  }

  void OnContextCreated(CefRefPtr<ClientAppRenderer> app,
                        CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefFrame> frame,
                        CefRefPtr<CefV8Context> context) OVERRIDE {
    message_router_->OnContextCreated(browser, frame, context);
	// Retrieve the context's window object.
	CefRefPtr<CefV8Value> object = context->GetGlobal();
	
	//Object:
	CefRefPtr<CefV8Value> obj = CefV8Value::CreateObject(NULL,NULL);
	
	/*CefRefPtr<CefV8Handler> handler = new MyV8Handler(this);
	object->SetValue("register",
		CefV8Value::CreateFunction("register", handler),
		V8_PROPERTY_ATTRIBUTE_NONE);;*/

	// Create an instance of my CefV8Handler object.
	CefRefPtr<CefV8Handler> handler = new CefV8HandlerImpl();

	// Create the "myfunc" function.
	CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("SEA_getSeaEnvironment", handler);
	CefRefPtr<CefV8Value> funcauth = CefV8Value::CreateFunction("SEA_seaAuth", handler);
	CefRefPtr<CefV8Value> funcoauth = CefV8Value::CreateFunction("SEA_userstatus", handler);
	CefRefPtr<CefV8Value> func3 = CefV8Value::CreateFunction("SEA_getSeaVersion", handler);
	CefRefPtr<CefV8Value> func4 = CefV8Value::CreateFunction("SEA_userEdit", handler);
	CefRefPtr<CefV8Value> func5 = CefV8Value::CreateFunction("SEA_actEquips", handler);
	CefRefPtr<CefV8Value> func6 = CefV8Value::CreateFunction("SEA_readEquips", handler);
	CefRefPtr<CefV8Value> funcretn = CefV8Value::CreateFunction("SEA_onreturn", handler);

	// Add the "myfunc" function to the "window" object.
	object->SetValue("getSeaEnvironment", func, V8_PROPERTY_ATTRIBUTE_NONE);
	object->SetValue("seaAuth", funcauth, V8_PROPERTY_ATTRIBUTE_NONE);
	object->SetValue("userEdit", func3, V8_PROPERTY_ATTRIBUTE_NONE);


	// Create an instance of my CefV8Handler object.
	CefRefPtr<CefV8Handler> handlerb = new CefV8HandlerImpl();

	// Create the "myfunc" function.
	CefRefPtr<CefV8Value> funcAlert = CefV8Value::CreateFunction("getSeaAlert", handlerb);
	CefRefPtr<CefV8Value> funcWxToken = CefV8Value::CreateFunction("getWxtoken", handlerb);

	// Add the "myfunc" function to the "window" object.
	obj->SetValue("alert", funcAlert, V8_PROPERTY_ATTRIBUTE_NONE);
	obj->SetValue("oauth", funcoauth, V8_PROPERTY_ATTRIBUTE_NONE);
	obj->SetValue("auth", funcauth, V8_PROPERTY_ATTRIBUTE_NONE);
	obj->SetValue("onReturn", funcretn, V8_PROPERTY_ATTRIBUTE_NONE);
	obj->SetValue("wxtoken", funcWxToken, V8_PROPERTY_ATTRIBUTE_NONE);
	obj->SetValue("credential", CefV8Value::CreateString(CefString("frame_x8_c7ep9")), V8_PROPERTY_ATTRIBUTE_NONE);
	// Create a new V8 string value. See the "Basic JS Types" section below.
	CefRefPtr<CefV8Value> versionStr = CefV8Value::CreateString("1.0.7a");
	object->SetValue("version", versionStr, V8_PROPERTY_ATTRIBUTE_NONE);
	// Add the string to the window object as "window.myval". See the "JS Objects" section below.
	CefRefPtr<CefV8Value> arr = CefV8Value::CreateArray(2);
	//Values are assigned to an array using the SetValue() method variant that takes an index as the first argument.
	arr->SetValue(0, CefV8Value::CreateString("login"));	// Add two values to the array.
	arr->SetValue(1, CefV8Value::CreateString("home"));
	obj->SetValue("privilige", arr, V8_PROPERTY_ATTRIBUTE_NONE); // 传递arr
	object->SetValue("sea", obj, V8_PROPERTY_ATTRIBUTE_NONE);
  }

 

  void OnContextReleased(CefRefPtr<ClientAppRenderer> app,
                         CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         CefRefPtr<CefV8Context> context) OVERRIDE {
    message_router_->OnContextReleased(browser, frame, context);
  }

  void OnFocusedNodeChanged(CefRefPtr<ClientAppRenderer> app,
                            CefRefPtr<CefBrowser> browser,
                            CefRefPtr<CefFrame> frame,
                            CefRefPtr<CefDOMNode> node) OVERRIDE {
    bool is_editable = (node.get() && node->IsEditable());
    if (is_editable != last_node_is_editable_) {
      // Notify the browser of the change in focused element type.
      last_node_is_editable_ = is_editable;
      CefRefPtr<CefProcessMessage> message =
          CefProcessMessage::Create(kFocusedNodeChangedMessage);
      message->GetArgumentList()->SetBool(0, is_editable);
      browser->SendProcessMessage(PID_BROWSER, message);
    }
  }

  bool OnProcessMessageReceived(CefRefPtr<ClientAppRenderer> app,
                                CefRefPtr<CefBrowser> browser,
                                CefProcessId source_process,
                                CefRefPtr<CefProcessMessage> message) OVERRIDE {
    return message_router_->OnProcessMessageReceived(browser, source_process,
                                                     message);
  }

 private:
  bool last_node_is_editable_;

  // Handles the renderer side of query routing.
  CefRefPtr<CefMessageRouterRendererSide> message_router_;

  DISALLOW_COPY_AND_ASSIGN(ClientRenderDelegate);
  IMPLEMENT_REFCOUNTING(ClientRenderDelegate);
};

}  // namespace

void CreateDelegates(ClientAppRenderer::DelegateSet& delegates) {
  delegates.insert(new ClientRenderDelegate);
}

}  // namespace renderer
}  // namespace client
