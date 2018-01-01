// JavaScript Document
function showTopNav(){
	var texts ='<div class="container-fluid">\n' +
'                    <div class="navbar-header">\n' +
'                        <button type="button" class="navbar-expand-toggle">\n' +
'                            <i class="fa fa-bars icon"></i>\n' +
'                        </button>\n' +
'                        <ol class="breadcrumb navbar-breadcrumb">\n' +
'                          <li class="active">Sea 智能工厂4.0 </li>\n' +
'                        </ol>\n' +
'<button type="button" class="navbar-right-expand-toggle pull-right visible-xs">\n' +
'            <i class="fa fa-th icon"></i>\n' +
'        </button>\n' +
'                    </div>\n' +
'                    <ul class="nav navbar-nav navbar-right">\n' +
'                        <button type="button" class="navbar-right-expand-toggle pull-right visible-xs">\n' +
'                            <i class="fa fa-times icon"></i>\n' +
'                        </button>\n' +
'                        <li class="dropdown">\n' +
'                            <a href="#" class="dropdown-toggle" data-toggle="dropdown" role="button" aria-expanded="false"><i class="fa fa-comments-o"></i></a>\n' +
'                            <ul class="dropdown-menu animated fadeInDown">\n' +
'                                <li class="title">\n' +
'                                    Notification <span class="badge pull-right">0</span>\n' +
'                                </li>\n' +
'                                <li class="message">\n' +
'                                    No new notification\n' +
'                                </li>\n' +
'                            </ul>\n' +
'                        </li>\n' +
'                        <li class="dropdown danger">\n' +
'                            <a href="#" class="dropdown-toggle" data-toggle="dropdown" role="button" aria-expanded="false"><i class="fa fa-star-half-o"></i> 4</a>\n' +
'                            <ul class="dropdown-menu danger  animated fadeInDown">\n' +
'                                <li class="title">\n' +
'                                    Notification <span class="badge pull-right">4</span>\n' +
'                                </li>\n' +
'                                <li>\n' +
'                                    <ul class="list-group notifications">\n' +
'                                        <a href="#">\n' +
'                                            <li class="list-group-item">\n' +
'                                                <span class="badge">1</span> <i class="fa fa-exclamation-circle icon"></i> new registration\n' +
'                                            </li>\n' +
'                                        </a>\n' +
'                                        <a href="#">\n' +
'                                            <li class="list-group-item">\n' +
'                                                <span class="badge success">1</span> <i class="fa fa-check icon"></i> new orders\n' +
'                                            </li>\n' +
'                                        </a>\n' +
'                                        <a href="#">\n' +
'                                            <li class="list-group-item">\n' +
'                                                <span class="badge danger">2</span> <i class="fa fa-comments icon"></i> customers messages\n' +
'                                            </li>\n' +
'                                        </a>\n' +
'                                        <a href="#">\n' +
'                                            <li class="list-group-item message">\n' +
'                                                view all\n' +
'                                            </li>\n' +
'                                        </a>\n' +
'                                    </ul>\n' +
'                                </li>\n' +
'                            </ul>\n' +
'                        </li>\n' +
'                        <li class="dropdown profile">\n' +
'                            <a href="#" class="dropdown-toggle" data-toggle="dropdown" role="button" aria-expanded="false">Emily Hart <span class="caret"></span></a>\n' +
'                            <ul class="dropdown-menu animated fadeInDown">\n' +
'                                <li class="profile-img">\n' +
'                                    <img src="img/profile/picjumbo.com_HNCK4153_resize.jpg" class="profile-img">\n' +
'                                </li>\n' +
'                                <li>\n' +
'                                    <div class="profile-info">\n' +
'                                        <h4 class="username" id="displayUserName"></h4>\n' +
'                                        <p>emily_hart@email.com</p>\n' +
'                                        <div class="btn-group margin-bottom-2x" role="group">\n' +
'                                            <button type="button" class="btn btn-default"><i class="fa fa-user"></i> Profile</button>\n' +
'                                            <button type="button" class="btn btn-default"><i class="fa fa-sign-out"></i> Logout</button>\n' +
'                                        </div>\n' +
'                                    </div>\n' +
'                                </li>\n' +
'                            </ul>\n' +
'                        </li>\n' +
'                    </ul>\n' +
'                </div>';
	document.getElementById("topNavBar").innerHTML = texts;
}