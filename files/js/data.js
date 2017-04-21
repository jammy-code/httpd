var actionUrl = document.domain;
var mtd_param;

if (actionUrl != null && actionUrl != "") {
    actionUrl = "http://" + actionUrl;
} else {
    alert("获取路由器网关IP失败！");
}



function userLogin(str) {
    var str_md5 = $.md5(str);
    $.ajax({
        type: "POST",
        timeout: 3000,
        url: actionUrl + "/authorize.api?actiont=login&usrid=" + str_md5,
        dataType: "JSON",
        success: function (data) {
            if (data.error == 0) {
                $.cookie('lstatus', true);
                if (mob == 1) {
                    $("#account_login").hide();
                }
                checkConfig();
            } else if (data.error == 10001) {
                getMsg('密码错误！');
                $("#user_login").text('重新登录');
                $("#user_login").attr("disabled", false);
            } else {
                getMsg('登录失败');
                $("#user_login").text('重新登录');
                $("#user_login").attr("disabled", false);
            }
        }, complete: function (XHR, TS) {
            XHR = null;
        },
        error: function (XHRequest, status, data) {
            getMsg(XHRequest.status);
            XHRequest.abort();
        }
    });
}