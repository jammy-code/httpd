
$(function () {

	/*首页登录*/
    //输入框焦点
    $(".input_s").focusin(function () {
        $(this).siblings("label").hide();
    }).focusout(function () {
        if ($(this).val() == '') {
            $(this).siblings("label").show();
        }
    })
	
    $("#user_login").click(function () {
	
        if ($("#login_usr").val() == '') {
            getMsg('请输入帐号', 1, '#login_usr');
            return;
        }
        if ($("#login_pwd").val() == '') {
            getMsg('请输入密码', 1, '#login_pwd');
            return;
        }
        var str = $("#login_usr").val()+":"+$("#login_pwd").val();
         getMsg(str, 1, '#login_usr');
        $("#user_login").text('正在登录');
        $("#user_login").attr("disabled", true);

        userLogin(str);
    });
	
});

/*
 * 检测是否是手机
 */
 
var browser = {
    versions: function () {
        var u = navigator.userAgent, app = navigator.appVersion;
        return {
            trident: u.indexOf('Trident') > -1, //IE内核 
            presto: u.indexOf('Presto') > -1, //opera内核 
            webKit: u.indexOf('AppleWebKit') > -1, //苹果、谷歌内核 
            gecko: u.indexOf('Gecko') > -1 && u.indexOf('KHTML') == -1, //火狐内核 
            mobile: !!u.match(/AppleWebKit.*Mobile.*/), //是否为移动终端 
            ios: !!u.match(/\(i[^;]+;( U;)? CPU.+Mac OS X/), //ios终端 
            android: u.indexOf('Android') > -1 || u.indexOf('Linux') > -1, //android终端或uc浏览器 
            iPhone: u.indexOf('iPhone') > -1, //是否为iPhone或者QQHD浏览器 
            iPad: u.indexOf('iPad') > -1, //是否iPad 
            webApp: u.indexOf('Safari') == -1 //是否web应该程序，没有头部与底部 
        };
    }(),
    language: (navigator.browserLanguage || navigator.language).toLowerCase()
}
 
function getMobile() {
    var mob = 0;
    if (browser.versions.mobile || browser.versions.android || browser.versions.iPhone) {
        mob = 1;
    }
    return mob;
}

function tips(content) {
    layer.open({
        title: false,
        shadeClose: false,
        content: content,
        style: 'background-color:#000;opacity:.75; box-shadow: none; text-align:center;color:#fff;',
        time: 1
    });
}

function getMsg(msg, type, sid) {
    var mob = getMobile();
    if (mob == 1) {
        return tips(msg);
    } else if (type == 1 && mob != 1) {
        layer.tips(msg, sid, {
            tips: [1, '#000000']
        });
    } else {
        return layer.msg(msg);
    }
}