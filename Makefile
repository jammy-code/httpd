

include $(TOPDIR)/Makefile.conf

PKG_VERSION:=
PKG_NAME:=jmsgd
PKG_SOURCE:=$(PKG_NAME)$(if $(PKG_VERSION),-$(PKG_VERSION))



define Package/httpd
  SECTION:=base
  CATEGORY:=Base system
  MAINTAINER:=
  TITLE:=httpd
  URL:=
  DEPENDS:=
  MENU:=1
endef


#CONFIG_COPY=y


define Extract/jmsgd
	mkdir -p $(PKG_SOURCE_DIR)
	cp -a $(TOPDIR)/vendor/Jammy/$(PKG_NAME)/src/* $(PKG_SOURCE_DIR)/
endef


include $(INCLUDE_DIR)/package.mk

$(eval $(call BuildPackage,httpd))


