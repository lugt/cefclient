
#include <sstream>
#include <string>
#include <algorithm>
#include <iostream>
#include <string>
#include <fstream>
#include <direct.h>

#include "include/cef_browser.h"
#include "include/cef_callback.h"
#include "include/cef_frame.h"
#include "include/cef_request.h"
#include "include/cef_resource_handler.h"
#include "include/cef_response.h"
#include "include/cef_scheme.h"
#include "include/base/cef_bind.h"
#include "include/cef_parser.h"
#include "include/cef_task.h"
#include "include/cef_trace.h"
#include "include/cef_web_plugin.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_stream_resource_handler.h"
#include "include/cef_crash_util.h"
#include "include/cef_dom.h"

#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_message_router.h"
#include "include/wrapper/cef_stream_resource_handler.h"
#include "tests/cefclient/browser/test_runner.h"
#include "tests/shared/browser/resource_util.h"

#include "tests/cefclient/browser/test_runner.h"


#include "tests/cefclient/browser/binding_test.h"
#include "tests/cefclient/browser/dialog_test.h"
#include "tests/cefclient/browser/drm_test.h"
#include "tests/cefclient/browser/main_context.h"
#include "tests/cefclient/browser/preferences_test.h"
#include "tests/cefclient/browser/resource.h"
#include "tests/cefclient/browser/response_filter_test.h"
#include "tests/cefclient/browser/root_window_manager.h"
#include "tests/cefclient/browser/scheme_test.h"
#include "tests/cefclient/browser/server_test.h"
#include "tests/cefclient/browser/urlrequest_test.h"
#include "tests/cefclient/browser/window_test.h"
#include "tests/shared/browser/resource_util.h"
#include "tests/cefclient/browser/main_context_impl.h"

// in CefV8HandlerImpl.h
class CefV8HandlerImpl : public CefV8Handler {
	public:
		CefV8HandlerImpl();
		~CefV8HandlerImpl();
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

		IMPLEMENT_REFCOUNTING(CefV8HandlerImpl);
};

class SeaCefUtils : public CefV8Context{
		

public :
	static std::string log_url;
	static std::string int2str(int num);
	static void setStatus(std::string bo);
	static std::string getStatus();
	static void setLoginResult(char * a, char * b);
	static std::string  status;
	static std::map<std::string, CefRefPtr<CefV8Value>> functionMap;
	int URLRequest(CefRefPtr<CefV8Value> json);
	int FileRequest(CefRefPtr<CefV8Value> json);
	void V8toMap(CefRefPtr<CefV8Value> hd, std::map<std::string, std::string> & map);
	static std::map<std::string, std::string> valueMap;
	static std::string SeaCefUtils::getMap(std::string v);
	static bool Auth(const CefString & name, CefRefPtr<CefV8Value> object, const CefV8ValueList & arguments, CefRefPtr<CefV8Value>& retval, CefString & exception);
	static bool checkAuthStatus(const CefString & name, CefRefPtr<CefV8Value> object, const CefV8ValueList & arguments, CefRefPtr<CefV8Value>& retval, CefString & exception);
public :
	static void registerCallback(int bw, std::string name, CefRefPtr<CefV8Value> val) {
		name = name + "-" +int2str(bw);
		functionMap.insert(std::pair<std::string, CefRefPtr<CefV8Value>>(name, val));
	}

	// Replace all instances of |from| with |to| in |str|.
	static std::string StringReplace(const std::string& str,
		const std::string& from,
		const std::string& to) {
		std::string result = str;
		std::string::size_type pos = 0;
		std::string::size_type from_len = from.length();
		std::string::size_type to_len = to.length();
		do {
			pos = result.find(from, pos);
			if (pos != std::string::npos) {
				result.replace(pos, from_len, to);
				pos += to_len;
			}
		} while (pos != std::string::npos);
		return result;
	}


	static std::string DumpRequestContents(CefRefPtr<CefRequest> request) {
		std::stringstream ss;

		ss << "URL: " << std::string(request->GetURL());
		ss << "\nMethod: " << std::string(request->GetMethod());

		CefRequest::HeaderMap headerMap;
		request->GetHeaderMap(headerMap);
		if (headerMap.size() > 0) {
			ss << "\nHeaders:";
			CefRequest::HeaderMap::const_iterator it = headerMap.begin();
			for (; it != headerMap.end(); ++it) {
				ss << "\n\t" << std::string((*it).first) << ": "
					<< std::string((*it).second);
			}
		}

		CefRefPtr<CefPostData> postData = request->GetPostData();
		if (postData.get()) {
			CefPostData::ElementVector elements;
			postData->GetElements(elements);
			if (elements.size() > 0) {
				ss << "\nPost Data:";
				CefRefPtr<CefPostDataElement> element;
				CefPostData::ElementVector::const_iterator it = elements.begin();
				for (; it != elements.end(); ++it) {
					element = (*it);
					if (element->GetType() == PDE_TYPE_BYTES) {
						// the element is composed of bytes
						ss << "\n\tBytes: ";
						if (element->GetBytesCount() == 0) {
							ss << "(empty)";
						}
						else {
							// retrieve the data.
							size_t size = element->GetBytesCount();
							char* bytes = new char[size];
							element->GetBytes(size, bytes);
							ss << std::string(bytes, size);
							delete[] bytes;
						}
					}
					else if (element->GetType() == PDE_TYPE_FILE) {
						ss << "\n\tFile: " << std::string(element->GetFile());
					}
				}
			}
		}

		return ss.str();
	}


	void RunGetSourceTest(CefRefPtr<CefBrowser> browser) {
		class Visitor : public CefStringVisitor {
		public:
			explicit Visitor(CefRefPtr<CefBrowser> browser) : browser_(browser) {}
			virtual void Visit(const CefString& string) OVERRIDE {
				std::string source = SeaCefUtils::StringReplace(string, "<", "&lt;");
				source = SeaCefUtils::StringReplace(source, ">", "&gt;");
				std::stringstream ss;
				ss << "<html><body bgcolor=\"white\">Haha Source:<pre>" << source
					<< "</pre></body></html>";
				browser_->GetMainFrame()->LoadString(ss.str(), "http://tests/getsource");
			}

		private:
			CefRefPtr<CefBrowser> browser_;
			IMPLEMENT_REFCOUNTING(Visitor);
		};

		browser->GetMainFrame()->GetSource(new Visitor(browser));
	}

	void RunGetTextTest(CefRefPtr<CefBrowser> browser) {
		class Visitor : public CefStringVisitor {
		public:
			explicit Visitor(CefRefPtr<CefBrowser> browser) : browser_(browser) {}
			virtual void Visit(const CefString& string) OVERRIDE {
				std::string text = StringReplace(string, "<", "&lt;");
				text = StringReplace(text, ">", "&gt;");
				std::stringstream ss;
				ss << "<html><body bgcolor=\"white\">Text:<pre>" << text
					<< "</pre></body></html>";
				browser_->GetMainFrame()->LoadString(ss.str(), "http://tests/gettext");
			}

		private:
			CefRefPtr<CefBrowser> browser_;
			IMPLEMENT_REFCOUNTING(Visitor);
		};

		browser->GetMainFrame()->GetText(new Visitor(browser));
	}

	void RunRequestTest(CefRefPtr<CefBrowser> browser) {
		// Create a new request
		CefRefPtr<CefRequest> request(CefRequest::Create());

		// Set the request URL
		request->SetURL("http://tests/request");

		// Add post data to the request.  The correct method and content-
		// type headers will be set by CEF.
		CefRefPtr<CefPostDataElement> postDataElement(CefPostDataElement::Create());
		std::string data = "arg1=val1&arg2=val2";
		postDataElement->SetToBytes(data.length(), data.c_str());
		CefRefPtr<CefPostData> postData(CefPostData::Create());
		postData->AddElement(postDataElement);
		request->SetPostData(postData);

		// Add a custom header
		CefRequest::HeaderMap headerMap;
		headerMap.insert(std::make_pair("X-My-Header", "My Header Value"));
		request->SetHeaderMap(headerMap);

		// Load the request
		browser->GetMainFrame()->LoadRequest(request);
	}

	void RunNewWindowTest(CefRefPtr<CefBrowser> browser) {
		client::RootWindowConfig config;
		config.with_controls = true;
		config.with_osr = browser->GetHost()->IsWindowRenderingDisabled();
		client::MainContext::Get()->GetRootWindowManager()->CreateRootWindow(config);
	}

	void RunPopupWindowTest(CefRefPtr<CefBrowser> browser) {
		browser->GetMainFrame()->ExecuteJavaScript(
			"window.open('sea://service');", "about:blank", 0);
	}


	void Alert(CefRefPtr<CefBrowser> browser, const std::string& message) {
		// Escape special characters in the message.
		std::string msg = StringReplace(message, "\\", "\\\\");
		msg = StringReplace(msg, "'", "\\'");

		// Execute a JavaScript alert().
		CefRefPtr<CefFrame> frame = browser->GetMainFrame();
		frame->ExecuteJavaScript("alert('" + msg + "');", frame->GetURL(), 0);
	}

	bool IsTestURL(const std::string& url, const std::string& path) {
		CefURLParts parts;
		CefParseURL(url, parts);

		const std::string& url_host = CefString(&parts.host);
		if (url_host != "localhost" && url_host != "tests")
			return false;

		const std::string& url_path = CefString(&parts.path);
		return url_path.find(path) == 0;
	}

	void RunPluginInfoTest(CefRefPtr<CefBrowser> browser) {
		class Visitor : public CefWebPluginInfoVisitor {
		public:
			explicit Visitor(CefRefPtr<CefBrowser> browser) : browser_(browser) {
				html_ =
					"<html><head><title>Plugin Info Test</title></head>"
					"<body bgcolor=\"white\">"
					"\n<b>Installed plugins:</b>";
			}
			~Visitor() {
				html_ += "\n</body></html>";

				// Load the html in the browser.
				browser_->GetMainFrame()->LoadString(html_, "http://tests/plugin_info");
			}

			virtual bool Visit(CefRefPtr<CefWebPluginInfo> info,
				int count,
				int total) OVERRIDE {
				html_ += "\n<br/><br/>Name: " + info->GetName().ToString() +
					"\n<br/>Description: " + info->GetDescription().ToString() +
					"\n<br/>Version: " + info->GetVersion().ToString() +
					"\n<br/>Path: " + info->GetPath().ToString();
				return true;
			}

		private:
			std::string html_;
			CefRefPtr<CefBrowser> browser_;
			IMPLEMENT_REFCOUNTING(Visitor);
		};

		CefVisitWebPluginInfo(new Visitor(browser));
	}

	static void ModifyZoom(CefRefPtr<CefBrowser> browser, double delta) {
		if (!CefCurrentlyOn(TID_UI)) {
			// Execute on the UI thread.
			CefPostTask(TID_UI, base::Bind(&ModifyZoom, browser, delta));
			return;
		}

		browser->GetHost()->SetZoomLevel(browser->GetHost()->GetZoomLevel() + delta);
	}


	CefRefPtr<CefStreamReader> GetDumpResponse(
		CefRefPtr<CefRequest> request,
		CefResponse::HeaderMap& response_headers) {
		std::string origin;

		// Extract the origin request header, if any. It will be specified for
		// cross-origin requests.
		{
			CefRequest::HeaderMap requestMap;
			request->GetHeaderMap(requestMap);

			CefRequest::HeaderMap::const_iterator it = requestMap.begin();
			for (; it != requestMap.end(); ++it) {
				std::string key = it->first;
				std::transform(key.begin(), key.end(), key.begin(), ::tolower);
				if (key == "origin") {
					origin = it->second;
					break;
				}
			}
		}

		if (!origin.empty() &&
			(origin.find("http://" + std::string("service")) == 0 ||
				origin.find("http://" + std::string("local")) == 0)) {
			// Allow cross-origin XMLHttpRequests from test origins.
			response_headers.insert(
				std::make_pair("Access-Control-Allow-Origin", origin));

			// Allow the custom header from the xmlhttprequest.html example.
			response_headers.insert(
				std::make_pair("Access-Control-Allow-Headers", "My-Custom-Header"));
		}

		const std::string& dump = DumpRequestContents(request);
		std::string str =
			"<html><body bgcolor=\"white\"><pre>" + dump + "</pre></body></html>";
		CefRefPtr<CefStreamReader> stream = CefStreamReader::CreateForData(
			static_cast<void*>(const_cast<char*>(str.c_str())), str.size());
		DCHECK(stream);
		return stream;
	}

	std::string GetDataURI(const std::string& data, const std::string& mime_type) {
		return "data:" + mime_type + ";base64," +
			CefURIEncode(CefBase64Encode(data.data(), data.size()), false)
			.ToString();
	}

	std::string GetErrorString(cef_errorcode_t code) {
		// Case condition that returns |code| as a string.
#define CASE(code) \
  case code:       \
    return #code

		switch (code) {
			CASE(ERR_NONE);
			CASE(ERR_FAILED);
			CASE(ERR_ABORTED);
			CASE(ERR_INVALID_ARGUMENT);
			CASE(ERR_INVALID_HANDLE);
			CASE(ERR_FILE_NOT_FOUND);
			CASE(ERR_TIMED_OUT);
			CASE(ERR_FILE_TOO_BIG);
			CASE(ERR_UNEXPECTED);
			CASE(ERR_ACCESS_DENIED);
			CASE(ERR_NOT_IMPLEMENTED);
			CASE(ERR_CONNECTION_CLOSED);
			CASE(ERR_CONNECTION_RESET);
			CASE(ERR_CONNECTION_REFUSED);
			CASE(ERR_CONNECTION_ABORTED);
			CASE(ERR_CONNECTION_FAILED);
			CASE(ERR_NAME_NOT_RESOLVED);
			CASE(ERR_INTERNET_DISCONNECTED);
			CASE(ERR_SSL_PROTOCOL_ERROR);
			CASE(ERR_ADDRESS_INVALID);
			CASE(ERR_ADDRESS_UNREACHABLE);
			CASE(ERR_SSL_CLIENT_AUTH_CERT_NEEDED);
			CASE(ERR_TUNNEL_CONNECTION_FAILED);
			CASE(ERR_NO_SSL_VERSIONS_ENABLED);
			CASE(ERR_SSL_VERSION_OR_CIPHER_MISMATCH);
			CASE(ERR_SSL_RENEGOTIATION_REQUESTED);
			CASE(ERR_CERT_COMMON_NAME_INVALID);
			CASE(ERR_CERT_DATE_INVALID);
			CASE(ERR_CERT_AUTHORITY_INVALID);
			CASE(ERR_CERT_CONTAINS_ERRORS);
			CASE(ERR_CERT_NO_REVOCATION_MECHANISM);
			CASE(ERR_CERT_UNABLE_TO_CHECK_REVOCATION);
			CASE(ERR_CERT_REVOKED);
			CASE(ERR_CERT_INVALID);
			CASE(ERR_CERT_END);
			CASE(ERR_INVALID_URL);
			CASE(ERR_DISALLOWED_URL_SCHEME);
			CASE(ERR_UNKNOWN_URL_SCHEME);
			CASE(ERR_TOO_MANY_REDIRECTS);
			CASE(ERR_UNSAFE_REDIRECT);
			CASE(ERR_UNSAFE_PORT);
			CASE(ERR_INVALID_RESPONSE);
			CASE(ERR_INVALID_CHUNKED_ENCODING);
			CASE(ERR_METHOD_NOT_SUPPORTED);
			CASE(ERR_UNEXPECTED_PROXY_AUTH);
			CASE(ERR_EMPTY_RESPONSE);
			CASE(ERR_RESPONSE_HEADERS_TOO_BIG);
			CASE(ERR_CACHE_MISS);
			CASE(ERR_INSECURE_RESPONSE);
		default:
			return "UNKNOWN";
		}
	}

	private:

		static std::string workingDirectory;

};


// Implementation of the resource handler for client requests.
class SeasResourceHandler : public CefResourceHandler {
public:
	SeasResourceHandler(CefRefPtr<CefBrowser> browser) {
		this->offset_ = 0;
	}

	SeasResourceHandler() : offset_(0) {}

	static int c0allback(void *NotUsed, int argc, char **argv, char **azColName) {
		int i;
		for (i = 0; i<argc; i++) {
			SeaCefUtils::setLoginResult(azColName[i],argv[i]);
			//printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		}
		SeaCefUtils::setStatus("resulted:true");
		//printf("\n");
		return 0;
	}

	virtual bool ProcessRequest(CefRefPtr<CefRequest> request,
		CefRefPtr<CefCallback> callback) OVERRIDE {
		CEF_REQUIRE_IO_THREAD();

		bool handled = false;
		isStreamed = false;

		std::string url = request->GetURL();
		if (strstr(url.c_str(), "handler.html") != NULL) {
			// Build the response html
			data_ =
				"<html><head><title>Client Scheme Handler</title></head>"
				"<body bgcolor=\"white\">"
				"This contents of this page page are served by the "
				"ClientSchemeHandler class handling the client:// protocol."
				"<br/>You should see an image:"
				"<br/><img src=\"client://tests/logo.png\"><pre>";

			// Output a string representation of the request
			const std::string& dump = SeaCefUtils::DumpRequestContents(request);
			data_.append(dump);

			data_.append(
				"</pre><br/>Try the test form:"
				"<form method=\"POST\" action=\"guan.html\">"
				"<input type=\"text\" name=\"field1\">"
				"<input type=\"text\" name=\"field2\">"
				"<input type=\"submit\">"
				"</form></body></html>");

			handled = true;

			// Set the resulting mime type
			mime_type_ = "text/html";
		}
		else if (strstr(url.c_str(), "logo.png") != NULL) {
			// Load the response image
			if (client::LoadBinaryResource("logo.png", data_)) {
				handled = true;
				// Set the resulting mime type
				mime_type_ = "image/png";
			}
		}
		else {
			CefURLParts parts;
			CefParseURL(url, parts);
			
			//const std::string& org_url_host = CefString(&parts.host);
			
			const std::string& org_url_path = CefString(&parts.path);
			std::string url_path = org_url_path; //org_url_path.substr(org_url_path.find("/", 2) + 1);

			std::string url_host = CefString(&parts.host);//org_url_path.substr(2, org_url_path.find("/", 2) - 2);

			if (strstr(url_host.c_str(), "global") != NULL) {
				request->SetURL(CefString("https://north.niimei.com"));
				handled = false;
				return false;
			}

			if (url_host == "local" || url_host == "game") {
				data_ = "";
				char *buffer;
				//也可以将buffer作为输出参数
				if ((buffer = getcwd(NULL, 0)) == NULL)
				{
					perror("getcwd error");
				}
				std::string url_n_path = CefString(buffer);
				if (url_host == "game") {
					url_n_path += CefString("\\release_egret\\");
				}else {
					url_n_path += CefString("\\btzero_10_flat\\");
				}
				url_n_path += SeaCefUtils::StringReplace(url_path, "/", "\\");
				//std::ifstream fin2(url_path);
				
				//url_path = "" +  url_path;
				mime_type_ = "application/octet-stream";
				std::ifstream t(url_n_path.c_str(),std::ios::binary);
				if (t.fail()) {
					t.close();
					data_ = "<title>SEA v20171229</title> <h1>404, Ooops!</h1> No Page Found locally, please contact Consumer Services";
					mime_type_ = "text/html";
				} else if(((int) url_path.find(".html")) >= 0 || ((int)url_path.find(".js")) >= 0 || ((int)url_path.find(".css")) >= 0){
					t = std::ifstream(url_n_path.c_str());
					std::string str((std::istreambuf_iterator<char>(t)),
						std::istreambuf_iterator<char>());
					t.close();
					data_ = str;
					totalLength = data_.length();
					int found = url_path.find(".html");
					if (found > 0) {
						mime_type_ = "text/html";
					}	else if ((found = url_path.find(".js")) > 0) {
						mime_type_ = "text/javascript";
					}	else if ((found = url_path.find(".css")) > 0) {
						mime_type_ = "text/css";
					}
				} else {
					//获取文件大小
					t.seekg(0, std::ios_base::end);
					totalLength = t.tellg();
					t.seekg(0);
					if (totalLength > 0) {
						tmp = (char *)malloc(totalLength + 2);
						t.read(tmp, totalLength);
						isStreamed = true;
						if (url_path.find(".jpg") > 0) {
							mime_type_ = "image/jpeg";
						}
						else if (url_path.find(".png") > 0) {
							mime_type_ = "image/png";
						}
						else {
							mime_type_ = "application/octet-stream";
						}
					}	else {
						isStreamed = false;
						data_ = "";
						totalLength = data_.length();
					}		
					t.close();
				}
				handled = true;
				// Set the resulting mime type
				//mime_type_ = "text/html";
			}
			else {

			}
		}

		if (handled) {
			// Indicate the headers are available.
			callback->Continue();
			return true;
		}

		return false;
	}

	virtual void GetResponseHeaders(CefRefPtr<CefResponse> response,
		int64& response_length,
		CefString& redirectUrl) OVERRIDE {
		CEF_REQUIRE_IO_THREAD();

		DCHECK(!data_.empty() || isStreamed == true && totalLength > 0);
		
		CefResponse::HeaderMap p;
		response->GetHeaderMap(p);
/*		if (p.find(CefString("Access-Control-Allow-Origin")).) {

		}*/
		//p.insert(std::make_pair("Access-Control-Allow-Origin", "*"));
		response->SetHeaderMap(p);		
		response->SetMimeType(mime_type_);
		response->SetStatus(200);

		// Set the resulting response length
		if (isStreamed) {
			response_length = totalLength;
		} else {
			response_length = data_.length();
		}
	}

	virtual void Cancel() OVERRIDE { CEF_REQUIRE_IO_THREAD(); }

	virtual bool ReadResponse(void* data_out,
		int bytes_to_read,
		int& bytes_read,
		CefRefPtr<CefCallback> callback) OVERRIDE {
		CEF_REQUIRE_IO_THREAD();

		bool has_data = false;
		bytes_read = 0;

		if (!isStreamed) {
			totalLength = data_.length();
			if (offset_ < totalLength) {
				// Copy the next block of data into the buffer.
				int transfer_size =
					std::min(bytes_to_read, static_cast<int>(totalLength - offset_));
				memcpy(data_out, data_.c_str() + offset_, transfer_size);
				offset_ += transfer_size;
				bytes_read = transfer_size;
				has_data = true;
			}
		} else {
			if (offset_ < totalLength) {
				// Copy the next block of data into the buffer.
				int transfer_size =
					std::min(bytes_to_read, static_cast<int>(totalLength - offset_));
				memcpy(data_out, tmp + offset_, transfer_size);
				offset_ += transfer_size;
				bytes_read = transfer_size;
				has_data = true;
			}
			if (!has_data) {
				free(tmp);
			}
		}

		

		return has_data;
	}

private:
	std::string data_;
	std::string mime_type_;
	size_t offset_;
	char *tmp;
	unsigned int totalLength;
	bool isStreamed;

	IMPLEMENT_REFCOUNTING(SeasResourceHandler);
};


// Implementation of the factory for creating client request handlers.
class SeaSchemeHandlerFactory : public CefSchemeHandlerFactory {
public:
	virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& scheme_name,
		CefRefPtr<CefRequest> request)
		OVERRIDE {
		// Return a new resource handler instance to handle the request.
		//"<html>OK, you are at your custom scheme </html>"
		//return new SeasResourceHandler();
		
		
		return new SeasResourceHandler(browser);

		/*
		
		
		const std::string& html_content = "<html><body>Hello!</body></html>";

		// Create a stream reader for |html_content|.
		CefRefPtr<CefStreamReader> stream =
			CefStreamReader::CreateForData(
				static_cast<void*>(const_cast<char*>(html_content.c_str())),
				html_content.size());

		// Constructor for HTTP status code 200 and no custom response headers.
		// There’s also a version of the constructor for custom status code and response headers.
		
			return new CefStreamResourceHandler("text/html", stream);
		*/
	}

	IMPLEMENT_REFCOUNTING(SeaSchemeHandlerFactory);
};