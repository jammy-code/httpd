include $(TOPDIR)/Makefile.conf

PKG_VERSION:=
PKG_NAME:=httpd
PKG_SOURCE:=$(PKG_NAME)$(if $(PKG_VERSION),-$(PKG_VERSION))

PKG_ORIGINAL_DIR:=$(TOPDIR)/vendor/Jammy/$(PKG_NAME)/src/

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


define Extract/httpd
	mkdir -p $(PKG_SOURCE_DIR)
endef

define Package/httpd/install
	$(INSTALL_DIR) $(1)/usr/www-root
	cp -a $(TOPDIR)/vendor/Jammy/$(PKG_NAME)/files/*  $(1)/usr/www-root/
endef



include $(INCLUDE_DIR)/package.mk

$(eval $(call BuildPackage,httpd))


