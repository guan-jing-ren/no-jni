#ifndef JFWD_DECL_HPP
#define JFWD_DECL_HPP

namespace java {}
[[maybe_unused]] inline constexpr jPackage java_{"java"};
namespace java::lang {}
[[maybe_unused]] inline constexpr jPackage java_lang = java_ / "lang";
namespace java::lang::annotation {}
[[maybe_unused]] inline constexpr jPackage java_lang_annotation = java_lang / "annotation";
namespace java::lang::instrument {}
[[maybe_unused]] inline constexpr jPackage java_lang_instrument = java_lang / "instrument";
namespace java::lang::invoke {}
[[maybe_unused]] inline constexpr jPackage java_lang_invoke = java_lang / "invoke";
namespace java::lang::management {}
[[maybe_unused]] inline constexpr jPackage java_lang_management = java_lang / "management";
namespace java::lang::ref {}
[[maybe_unused]] inline constexpr jPackage java_lang_ref = java_lang / "ref";
namespace java::lang::reflect {}
[[maybe_unused]] inline constexpr jPackage java_lang_reflect = java_lang / "reflect";
namespace java::applet {}
[[maybe_unused]] inline constexpr jPackage java_applet = java_ / "applet";
namespace java::awt {}
[[maybe_unused]] inline constexpr jPackage java_awt = java_ / "awt";
namespace java::awt::color {}
[[maybe_unused]] inline constexpr jPackage java_awt_color = java_awt / "color";
namespace java::awt::datatransfer {}
[[maybe_unused]] inline constexpr jPackage java_awt_datatransfer = java_awt / "datatransfer";
namespace java::awt::dnd {}
[[maybe_unused]] inline constexpr jPackage java_awt_dnd = java_awt / "dnd";
namespace java::awt::dnd::peer {}
[[maybe_unused]] inline constexpr jPackage java_awt_dnd_peer = java_awt_dnd / "peer";
namespace java::awt::event {}
[[maybe_unused]] inline constexpr jPackage java_awt_event = java_awt / "event";
namespace java::awt::font {}
[[maybe_unused]] inline constexpr jPackage java_awt_font = java_awt / "font";
namespace java::awt::geom {}
[[maybe_unused]] inline constexpr jPackage java_awt_geom = java_awt / "geom";
namespace java::awt::im {}
[[maybe_unused]] inline constexpr jPackage java_awt_im = java_awt / "im";
namespace java::awt::im::spi {}
[[maybe_unused]] inline constexpr jPackage java_awt_im_spi = java_awt_im / "spi";
namespace java::awt::image {}
[[maybe_unused]] inline constexpr jPackage java_awt_image = java_awt / "image";
namespace java::awt::image::renderable {}
[[maybe_unused]] inline constexpr jPackage java_awt_image_renderable = java_awt_image / "renderable";
namespace java::awt::peer {}
[[maybe_unused]] inline constexpr jPackage java_awt_peer = java_awt / "peer";
namespace java::awt::print {}
[[maybe_unused]] inline constexpr jPackage java_awt_print = java_awt / "print";
namespace java::beans {}
[[maybe_unused]] inline constexpr jPackage java_beans = java_ / "beans";
namespace java::beans::beancontext {}
[[maybe_unused]] inline constexpr jPackage java_beans_beancontext = java_beans / "beancontext";
namespace java::io {}
[[maybe_unused]] inline constexpr jPackage java_io = java_ / "io";
namespace java::math {}
[[maybe_unused]] inline constexpr jPackage java_math = java_ / "math";
namespace java::net {}
[[maybe_unused]] inline constexpr jPackage java_net = java_ / "net";
namespace java::nio {}
[[maybe_unused]] inline constexpr jPackage java_nio = java_ / "nio";
namespace java::nio::channels {}
[[maybe_unused]] inline constexpr jPackage java_nio_channels = java_nio / "channels";
namespace java::nio::channels::spi {}
[[maybe_unused]] inline constexpr jPackage java_nio_channels_spi = java_nio_channels / "spi";
namespace java::nio::charset {}
[[maybe_unused]] inline constexpr jPackage java_nio_charset = java_nio / "charset";
namespace java::nio::charset::spi {}
[[maybe_unused]] inline constexpr jPackage java_nio_charset_spi = java_nio_charset / "spi";
namespace java::nio::file {}
[[maybe_unused]] inline constexpr jPackage java_nio_file = java_nio / "file";
namespace java::nio::file::attribute {}
[[maybe_unused]] inline constexpr jPackage java_nio_file_attribute = java_nio_file / "attribute";
namespace java::nio::file::spi {}
[[maybe_unused]] inline constexpr jPackage java_nio_file_spi = java_nio_file / "spi";
namespace java::rmi {}
[[maybe_unused]] inline constexpr jPackage java_rmi = java_ / "rmi";
namespace java::rmi::activation {}
[[maybe_unused]] inline constexpr jPackage java_rmi_activation = java_rmi / "activation";
namespace java::rmi::dgc {}
[[maybe_unused]] inline constexpr jPackage java_rmi_dgc = java_rmi / "dgc";
namespace java::rmi::registry {}
[[maybe_unused]] inline constexpr jPackage java_rmi_registry = java_rmi / "registry";
namespace java::rmi::server {}
[[maybe_unused]] inline constexpr jPackage java_rmi_server = java_rmi / "server";
namespace java::security {}
[[maybe_unused]] inline constexpr jPackage java_security = java_ / "security";
namespace java::security::acl {}
[[maybe_unused]] inline constexpr jPackage java_security_acl = java_security / "acl";
namespace java::security::cert {}
[[maybe_unused]] inline constexpr jPackage java_security_cert = java_security / "cert";
namespace java::security::interfaces {}
[[maybe_unused]] inline constexpr jPackage java_security_interfaces = java_security / "interfaces";
namespace java::security::spec {}
[[maybe_unused]] inline constexpr jPackage java_security_spec = java_security / "spec";
namespace java::sql {}
[[maybe_unused]] inline constexpr jPackage java_sql = java_ / "sql";
namespace java::text {}
[[maybe_unused]] inline constexpr jPackage java_text = java_ / "text";
namespace java::text::spi {}
[[maybe_unused]] inline constexpr jPackage java_text_spi = java_text / "spi";
namespace java::time {}
[[maybe_unused]] inline constexpr jPackage java_time = java_ / "time";
namespace java::time::chrono {}
[[maybe_unused]] inline constexpr jPackage java_time_chrono = java_time / "chrono";
namespace java::time::format {}
[[maybe_unused]] inline constexpr jPackage java_time_format = java_time / "format";
namespace java::time::temporal {}
[[maybe_unused]] inline constexpr jPackage java_time_temporal = java_time / "temporal";
namespace java::time::zone {}
[[maybe_unused]] inline constexpr jPackage java_time_zone = java_time / "zone";
namespace java::util {}
[[maybe_unused]] inline constexpr jPackage java_util = java_ / "util";
namespace java::util::concurrent {}
[[maybe_unused]] inline constexpr jPackage java_util_concurrent = java_util / "concurrent";
namespace java::util::concurrent::atomic {}
[[maybe_unused]] inline constexpr jPackage java_util_concurrent_atomic = java_util_concurrent / "atomic";
namespace java::util::concurrent::locks {}
[[maybe_unused]] inline constexpr jPackage java_util_concurrent_locks = java_util_concurrent / "locks";
namespace java::util::function {}
[[maybe_unused]] inline constexpr jPackage java_util_function = java_util / "function";
namespace java::util::jar {}
[[maybe_unused]] inline constexpr jPackage java_util_jar = java_util / "jar";
namespace java::util::logging {}
[[maybe_unused]] inline constexpr jPackage java_util_logging = java_util / "logging";
namespace java::util::prefs {}
[[maybe_unused]] inline constexpr jPackage java_util_prefs = java_util / "prefs";
namespace java::util::regex {}
[[maybe_unused]] inline constexpr jPackage java_util_regex = java_util / "regex";
namespace java::util::spi {}
[[maybe_unused]] inline constexpr jPackage java_util_spi = java_util / "spi";
namespace java::util::stream {}
[[maybe_unused]] inline constexpr jPackage java_util_stream = java_util / "stream";
namespace java::util::zip {}
[[maybe_unused]] inline constexpr jPackage java_util_zip = java_util / "zip";
namespace javax {}
[[maybe_unused]] inline constexpr jPackage javax_{"javax"};
namespace javax::accessibility {}
[[maybe_unused]] inline constexpr jPackage javax_accessibility = javax_ / "accessibility";
namespace javax::activation {}
[[maybe_unused]] inline constexpr jPackage javax_activation = javax_ / "activation";
namespace javax::activity {}
[[maybe_unused]] inline constexpr jPackage javax_activity = javax_ / "activity";
namespace javax::annotation {}
[[maybe_unused]] inline constexpr jPackage javax_annotation = javax_ / "annotation";
namespace javax::annotation::processing {}
[[maybe_unused]] inline constexpr jPackage javax_annotation_processing = javax_annotation / "processing";
namespace javax::crypto {}
[[maybe_unused]] inline constexpr jPackage javax_crypto = javax_ / "crypto";
namespace javax::crypto::interfaces {}
[[maybe_unused]] inline constexpr jPackage javax_crypto_interfaces = javax_crypto / "interfaces";
namespace javax::crypto::spec {}
[[maybe_unused]] inline constexpr jPackage javax_crypto_spec = javax_crypto / "spec";
namespace javax::imageio {}
[[maybe_unused]] inline constexpr jPackage javax_imageio = javax_ / "imageio";
namespace javax::imageio::event {}
[[maybe_unused]] inline constexpr jPackage javax_imageio_event = javax_imageio / "event";
namespace javax::imageio::metadata {}
[[maybe_unused]] inline constexpr jPackage javax_imageio_metadata = javax_imageio / "metadata";
namespace javax::imageio::plugins {}
[[maybe_unused]] inline constexpr jPackage javax_imageio_plugins = javax_imageio / "plugins";
namespace javax::imageio::plugins::bmp {}
[[maybe_unused]] inline constexpr jPackage javax_imageio_plugins_bmp = javax_imageio_plugins / "bmp";
namespace javax::imageio::plugins::jpeg {}
[[maybe_unused]] inline constexpr jPackage javax_imageio_plugins_jpeg = javax_imageio_plugins / "jpeg";
namespace javax::imageio::spi {}
[[maybe_unused]] inline constexpr jPackage javax_imageio_spi = javax_imageio / "spi";
namespace javax::imageio::stream {}
[[maybe_unused]] inline constexpr jPackage javax_imageio_stream = javax_imageio / "stream";
namespace javax::jws {}
[[maybe_unused]] inline constexpr jPackage javax_jws = javax_ / "jws";
namespace javax::jws::soap {}
[[maybe_unused]] inline constexpr jPackage javax_jws_soap = javax_jws / "soap";
namespace javax::lang {}
[[maybe_unused]] inline constexpr jPackage javax_lang = javax_ / "lang";
namespace javax::lang::model {}
[[maybe_unused]] inline constexpr jPackage javax_lang_model = javax_lang / "model";
namespace javax::lang::model::element {}
[[maybe_unused]] inline constexpr jPackage javax_lang_model_element = javax_lang_model / "element";
namespace javax::lang::model::type {}
[[maybe_unused]] inline constexpr jPackage javax_lang_model_type = javax_lang_model / "type";
namespace javax::lang::model::util {}
[[maybe_unused]] inline constexpr jPackage javax_lang_model_util = javax_lang_model / "util";
namespace javax::management {}
[[maybe_unused]] inline constexpr jPackage javax_management = javax_ / "management";
namespace javax::management::loading {}
[[maybe_unused]] inline constexpr jPackage javax_management_loading = javax_management / "loading";
namespace javax::management::modelmbean {}
[[maybe_unused]] inline constexpr jPackage javax_management_modelmbean = javax_management / "modelmbean";
namespace javax::management::monitor {}
[[maybe_unused]] inline constexpr jPackage javax_management_monitor = javax_management / "monitor";
namespace javax::management::openmbean {}
[[maybe_unused]] inline constexpr jPackage javax_management_openmbean = javax_management / "openmbean";
namespace javax::management::relation {}
[[maybe_unused]] inline constexpr jPackage javax_management_relation = javax_management / "relation";
namespace javax::management::remote {}
[[maybe_unused]] inline constexpr jPackage javax_management_remote = javax_management / "remote";
namespace javax::management::remote::rmi {}
[[maybe_unused]] inline constexpr jPackage javax_management_remote_rmi = javax_management_remote / "rmi";
namespace javax::management::timer {}
[[maybe_unused]] inline constexpr jPackage javax_management_timer = javax_management / "timer";
namespace javax::naming {}
[[maybe_unused]] inline constexpr jPackage javax_naming = javax_ / "naming";
namespace javax::naming::directory {}
[[maybe_unused]] inline constexpr jPackage javax_naming_directory = javax_naming / "directory";
namespace javax::naming::event {}
[[maybe_unused]] inline constexpr jPackage javax_naming_event = javax_naming / "event";
namespace javax::naming::ldap {}
[[maybe_unused]] inline constexpr jPackage javax_naming_ldap = javax_naming / "ldap";
namespace javax::naming::spi {}
[[maybe_unused]] inline constexpr jPackage javax_naming_spi = javax_naming / "spi";
namespace javax::net {}
[[maybe_unused]] inline constexpr jPackage javax_net = javax_ / "net";
namespace javax::net::ssl {}
[[maybe_unused]] inline constexpr jPackage javax_net_ssl = javax_net / "ssl";
namespace javax::print {}
[[maybe_unused]] inline constexpr jPackage javax_print = javax_ / "print";
namespace javax::print::attribute {}
[[maybe_unused]] inline constexpr jPackage javax_print_attribute = javax_print / "attribute";
namespace javax::print::attribute::standard {}
[[maybe_unused]] inline constexpr jPackage javax_print_attribute_standard = javax_print_attribute / "standard";
namespace javax::print::event {}
[[maybe_unused]] inline constexpr jPackage javax_print_event = javax_print / "event";
namespace javax::rmi {}
[[maybe_unused]] inline constexpr jPackage javax_rmi = javax_ / "rmi";
namespace javax::rmi::CORBA {}
[[maybe_unused]] inline constexpr jPackage javax_rmi_CORBA = javax_rmi / "CORBA";
namespace javax::rmi::ssl {}
[[maybe_unused]] inline constexpr jPackage javax_rmi_ssl = javax_rmi / "ssl";
namespace javax::script {}
[[maybe_unused]] inline constexpr jPackage javax_script = javax_ / "script";
namespace javax::security {}
[[maybe_unused]] inline constexpr jPackage javax_security = javax_ / "security";
namespace javax::security::auth {}
[[maybe_unused]] inline constexpr jPackage javax_security_auth = javax_security / "auth";
namespace javax::security::auth::callback {}
[[maybe_unused]] inline constexpr jPackage javax_security_auth_callback = javax_security_auth / "callback";
namespace javax::security::auth::kerberos {}
[[maybe_unused]] inline constexpr jPackage javax_security_auth_kerberos = javax_security_auth / "kerberos";
namespace javax::security::auth::login {}
[[maybe_unused]] inline constexpr jPackage javax_security_auth_login = javax_security_auth / "login";
namespace javax::security::auth::spi {}
[[maybe_unused]] inline constexpr jPackage javax_security_auth_spi = javax_security_auth / "spi";
namespace javax::security::auth::x500 {}
[[maybe_unused]] inline constexpr jPackage javax_security_auth_x500 = javax_security_auth / "x500";
namespace javax::security::cert {}
[[maybe_unused]] inline constexpr jPackage javax_security_cert = javax_security / "cert";
namespace javax::security::sasl {}
[[maybe_unused]] inline constexpr jPackage javax_security_sasl = javax_security / "sasl";
namespace javax::smartcardio {}
[[maybe_unused]] inline constexpr jPackage javax_smartcardio = javax_ / "smartcardio";
namespace javax::sound {}
[[maybe_unused]] inline constexpr jPackage javax_sound = javax_ / "sound";
namespace javax::sound::midi {}
[[maybe_unused]] inline constexpr jPackage javax_sound_midi = javax_sound / "midi";
namespace javax::sound::midi::spi {}
[[maybe_unused]] inline constexpr jPackage javax_sound_midi_spi = javax_sound_midi / "spi";
namespace javax::sound::sampled {}
[[maybe_unused]] inline constexpr jPackage javax_sound_sampled = javax_sound / "sampled";
namespace javax::sound::sampled::spi {}
[[maybe_unused]] inline constexpr jPackage javax_sound_sampled_spi = javax_sound_sampled / "spi";
namespace javax::sql {}
[[maybe_unused]] inline constexpr jPackage javax_sql = javax_ / "sql";
namespace javax::sql::rowset {}
[[maybe_unused]] inline constexpr jPackage javax_sql_rowset = javax_sql / "rowset";
namespace javax::sql::rowset::serial {}
[[maybe_unused]] inline constexpr jPackage javax_sql_rowset_serial = javax_sql_rowset / "serial";
namespace javax::sql::rowset::spi {}
[[maybe_unused]] inline constexpr jPackage javax_sql_rowset_spi = javax_sql_rowset / "spi";
namespace javax::swing {}
[[maybe_unused]] inline constexpr jPackage javax_swing = javax_ / "swing";
namespace javax::swing::border {}
[[maybe_unused]] inline constexpr jPackage javax_swing_border = javax_swing / "border";
namespace javax::swing::colorchooser {}
[[maybe_unused]] inline constexpr jPackage javax_swing_colorchooser = javax_swing / "colorchooser";
namespace javax::swing::event {}
[[maybe_unused]] inline constexpr jPackage javax_swing_event = javax_swing / "event";
namespace javax::swing::filechooser {}
[[maybe_unused]] inline constexpr jPackage javax_swing_filechooser = javax_swing / "filechooser";
namespace javax::swing::plaf {}
[[maybe_unused]] inline constexpr jPackage javax_swing_plaf = javax_swing / "plaf";
namespace javax::swing::plaf::basic {}
[[maybe_unused]] inline constexpr jPackage javax_swing_plaf_basic = javax_swing_plaf / "basic";
namespace javax::swing::plaf::metal {}
[[maybe_unused]] inline constexpr jPackage javax_swing_plaf_metal = javax_swing_plaf / "metal";
namespace javax::swing::plaf::multi {}
[[maybe_unused]] inline constexpr jPackage javax_swing_plaf_multi = javax_swing_plaf / "multi";
namespace javax::swing::plaf::nimbus {}
[[maybe_unused]] inline constexpr jPackage javax_swing_plaf_nimbus = javax_swing_plaf / "nimbus";
namespace javax::swing::plaf::synth {}
[[maybe_unused]] inline constexpr jPackage javax_swing_plaf_synth = javax_swing_plaf / "synth";
namespace javax::swing::table {}
[[maybe_unused]] inline constexpr jPackage javax_swing_table = javax_swing / "table";
namespace javax::swing::text {}
[[maybe_unused]] inline constexpr jPackage javax_swing_text = javax_swing / "text";
namespace javax::swing::text::html {}
[[maybe_unused]] inline constexpr jPackage javax_swing_text_html = javax_swing_text / "html";
namespace javax::swing::text::html::parser {}
[[maybe_unused]] inline constexpr jPackage javax_swing_text_html_parser = javax_swing_text_html / "parser";
namespace javax::swing::text::rtf {}
[[maybe_unused]] inline constexpr jPackage javax_swing_text_rtf = javax_swing_text / "rtf";
namespace javax::swing::tree {}
[[maybe_unused]] inline constexpr jPackage javax_swing_tree = javax_swing / "tree";
namespace javax::swing::undo {}
[[maybe_unused]] inline constexpr jPackage javax_swing_undo = javax_swing / "undo";
namespace javax::tools {}
[[maybe_unused]] inline constexpr jPackage javax_tools = javax_ / "tools";
namespace javax::transaction {}
[[maybe_unused]] inline constexpr jPackage javax_transaction = javax_ / "transaction";
namespace javax::transaction::xa {}
[[maybe_unused]] inline constexpr jPackage javax_transaction_xa = javax_transaction / "xa";
namespace javax::xml {}
[[maybe_unused]] inline constexpr jPackage javax_xml = javax_ / "xml";
namespace javax::xml::bind {}
[[maybe_unused]] inline constexpr jPackage javax_xml_bind = javax_xml / "bind";
namespace javax::xml::bind::annotation {}
[[maybe_unused]] inline constexpr jPackage javax_xml_bind_annotation = javax_xml_bind / "annotation";
namespace javax::xml::bind::annotation::adapters {}
[[maybe_unused]] inline constexpr jPackage javax_xml_bind_annotation_adapters = javax_xml_bind_annotation / "adapters";
namespace javax::xml::bind::attachment {}
[[maybe_unused]] inline constexpr jPackage javax_xml_bind_attachment = javax_xml_bind / "attachment";
namespace javax::xml::bind::helpers {}
[[maybe_unused]] inline constexpr jPackage javax_xml_bind_helpers = javax_xml_bind / "helpers";
namespace javax::xml::bind::util {}
[[maybe_unused]] inline constexpr jPackage javax_xml_bind_util = javax_xml_bind / "util";
namespace javax::xml::crypto {}
[[maybe_unused]] inline constexpr jPackage javax_xml_crypto = javax_xml / "crypto";
namespace javax::xml::crypto::dom {}
[[maybe_unused]] inline constexpr jPackage javax_xml_crypto_dom = javax_xml_crypto / "dom";
namespace javax::xml::crypto::dsig {}
[[maybe_unused]] inline constexpr jPackage javax_xml_crypto_dsig = javax_xml_crypto / "dsig";
namespace javax::xml::crypto::dsig::dom {}
[[maybe_unused]] inline constexpr jPackage javax_xml_crypto_dsig_dom = javax_xml_crypto_dsig / "dom";
namespace javax::xml::crypto::dsig::keyinfo {}
[[maybe_unused]] inline constexpr jPackage javax_xml_crypto_dsig_keyinfo = javax_xml_crypto_dsig / "keyinfo";
namespace javax::xml::crypto::dsig::spec {}
[[maybe_unused]] inline constexpr jPackage javax_xml_crypto_dsig_spec = javax_xml_crypto_dsig / "spec";
namespace javax::xml::datatype {}
[[maybe_unused]] inline constexpr jPackage javax_xml_datatype = javax_xml / "datatype";
namespace javax::xml::namespace_ {}
[[maybe_unused]] inline constexpr jPackage javax_xml_namespace_ = javax_xml / "namespace_";
namespace javax::xml::parsers {}
[[maybe_unused]] inline constexpr jPackage javax_xml_parsers = javax_xml / "parsers";
namespace javax::xml::soap {}
[[maybe_unused]] inline constexpr jPackage javax_xml_soap = javax_xml / "soap";
namespace javax::xml::stream {}
[[maybe_unused]] inline constexpr jPackage javax_xml_stream = javax_xml / "stream";
namespace javax::xml::stream::events {}
[[maybe_unused]] inline constexpr jPackage javax_xml_stream_events = javax_xml_stream / "events";
namespace javax::xml::stream::util {}
[[maybe_unused]] inline constexpr jPackage javax_xml_stream_util = javax_xml_stream / "util";
namespace javax::xml::transform {}
[[maybe_unused]] inline constexpr jPackage javax_xml_transform = javax_xml / "transform";
namespace javax::xml::transform::dom {}
[[maybe_unused]] inline constexpr jPackage javax_xml_transform_dom = javax_xml_transform / "dom";
namespace javax::xml::transform::sax {}
[[maybe_unused]] inline constexpr jPackage javax_xml_transform_sax = javax_xml_transform / "sax";
namespace javax::xml::transform::stax {}
[[maybe_unused]] inline constexpr jPackage javax_xml_transform_stax = javax_xml_transform / "stax";
namespace javax::xml::transform::stream {}
[[maybe_unused]] inline constexpr jPackage javax_xml_transform_stream = javax_xml_transform / "stream";
namespace javax::xml::validation {}
[[maybe_unused]] inline constexpr jPackage javax_xml_validation = javax_xml / "validation";
namespace javax::xml::ws {}
[[maybe_unused]] inline constexpr jPackage javax_xml_ws = javax_xml / "ws";
namespace javax::xml::ws::handler {}
[[maybe_unused]] inline constexpr jPackage javax_xml_ws_handler = javax_xml_ws / "handler";
namespace javax::xml::ws::handler::soap {}
[[maybe_unused]] inline constexpr jPackage javax_xml_ws_handler_soap = javax_xml_ws_handler / "soap";
namespace javax::xml::ws::http {}
[[maybe_unused]] inline constexpr jPackage javax_xml_ws_http = javax_xml_ws / "http";
namespace javax::xml::ws::soap {}
[[maybe_unused]] inline constexpr jPackage javax_xml_ws_soap = javax_xml_ws / "soap";
namespace javax::xml::ws::spi {}
[[maybe_unused]] inline constexpr jPackage javax_xml_ws_spi = javax_xml_ws / "spi";
namespace javax::xml::ws::spi::http {}
[[maybe_unused]] inline constexpr jPackage javax_xml_ws_spi_http = javax_xml_ws_spi / "http";
namespace javax::xml::ws::wsaddressing {}
[[maybe_unused]] inline constexpr jPackage javax_xml_ws_wsaddressing = javax_xml_ws / "wsaddressing";
namespace javax::xml::xpath {}
[[maybe_unused]] inline constexpr jPackage javax_xml_xpath = javax_xml / "xpath";
namespace org {}
[[maybe_unused]] inline constexpr jPackage org_{"org"};
namespace org::eclipse {}
[[maybe_unused]] inline constexpr jPackage org_eclipse = org_ / "eclipse";
namespace org::eclipse::swt {}
[[maybe_unused]] inline constexpr jPackage org_eclipse_swt = org_eclipse / "swt";
namespace org::eclipse::swt::accessibility {}
[[maybe_unused]] inline constexpr jPackage org_eclipse_swt_accessibility = org_eclipse_swt / "accessibility";
namespace org::eclipse::swt::awt {}
[[maybe_unused]] inline constexpr jPackage org_eclipse_swt_awt = org_eclipse_swt / "awt";
namespace org::eclipse::swt::browser {}
[[maybe_unused]] inline constexpr jPackage org_eclipse_swt_browser = org_eclipse_swt / "browser";
namespace org::eclipse::swt::custom {}
[[maybe_unused]] inline constexpr jPackage org_eclipse_swt_custom = org_eclipse_swt / "custom";
namespace org::eclipse::swt::dnd {}
[[maybe_unused]] inline constexpr jPackage org_eclipse_swt_dnd = org_eclipse_swt / "dnd";
namespace org::eclipse::swt::events {}
[[maybe_unused]] inline constexpr jPackage org_eclipse_swt_events = org_eclipse_swt / "events";
namespace org::eclipse::swt::graphics {}
[[maybe_unused]] inline constexpr jPackage org_eclipse_swt_graphics = org_eclipse_swt / "graphics";
namespace org::eclipse::swt::layout {}
[[maybe_unused]] inline constexpr jPackage org_eclipse_swt_layout = org_eclipse_swt / "layout";
namespace org::eclipse::swt::opengl {}
[[maybe_unused]] inline constexpr jPackage org_eclipse_swt_opengl = org_eclipse_swt / "opengl";
namespace org::eclipse::swt::printing {}
[[maybe_unused]] inline constexpr jPackage org_eclipse_swt_printing = org_eclipse_swt / "printing";
namespace org::eclipse::swt::program {}
[[maybe_unused]] inline constexpr jPackage org_eclipse_swt_program = org_eclipse_swt / "program";
namespace org::eclipse::swt::widgets {}
[[maybe_unused]] inline constexpr jPackage org_eclipse_swt_widgets = org_eclipse_swt / "widgets";
namespace org::ietf {}
[[maybe_unused]] inline constexpr jPackage org_ietf = org_ / "ietf";
namespace org::ietf::jgss {}
[[maybe_unused]] inline constexpr jPackage org_ietf_jgss = org_ietf / "jgss";
namespace org::omg {}
[[maybe_unused]] inline constexpr jPackage org_omg = org_ / "omg";
namespace org::omg::CORBA {}
[[maybe_unused]] inline constexpr jPackage org_omg_CORBA = org_omg / "CORBA";
namespace org::omg::CORBA_2 {}
[[maybe_unused]] inline constexpr jPackage org_omg_CORBA_2 = org_omg / "CORBA_2";
namespace org::omg::CORBA::DynAnyPackage {}
[[maybe_unused]] inline constexpr jPackage org_omg_CORBA_DynAnyPackage = org_omg_CORBA / "DynAnyPackage";
namespace org::omg::CORBA::ORBPackage {}
[[maybe_unused]] inline constexpr jPackage org_omg_CORBA_ORBPackage = org_omg_CORBA / "ORBPackage";
namespace org::omg::CORBA::TypeCodePackage {}
[[maybe_unused]] inline constexpr jPackage org_omg_CORBA_TypeCodePackage = org_omg_CORBA / "TypeCodePackage";
namespace org::omg::CORBA::portable {}
[[maybe_unused]] inline constexpr jPackage org_omg_CORBA_portable = org_omg_CORBA / "portable";
namespace org::omg::CORBA_2_3 {}
[[maybe_unused]] inline constexpr jPackage org_omg_CORBA_2_3 = org_omg_CORBA_2 / "3";
namespace org::omg::CORBA_2_3::portable {}
[[maybe_unused]] inline constexpr jPackage org_omg_CORBA_2_3_portable = org_omg_CORBA_2_3 / "portable";
namespace org::omg::CosNaming {}
[[maybe_unused]] inline constexpr jPackage org_omg_CosNaming = org_omg / "CosNaming";
namespace org::omg::CosNaming::NamingContextExtPackage {}
[[maybe_unused]] inline constexpr jPackage org_omg_CosNaming_NamingContextExtPackage = org_omg_CosNaming / "NamingContextExtPackage";
namespace org::omg::CosNaming::NamingContextPackage {}
[[maybe_unused]] inline constexpr jPackage org_omg_CosNaming_NamingContextPackage = org_omg_CosNaming / "NamingContextPackage";
namespace org::omg::Dynamic {}
[[maybe_unused]] inline constexpr jPackage org_omg_Dynamic = org_omg / "Dynamic";
namespace org::omg::DynamicAny {}
[[maybe_unused]] inline constexpr jPackage org_omg_DynamicAny = org_omg / "DynamicAny";
namespace org::omg::DynamicAny::DynAnyFactoryPackage {}
[[maybe_unused]] inline constexpr jPackage org_omg_DynamicAny_DynAnyFactoryPackage = org_omg_DynamicAny / "DynAnyFactoryPackage";
namespace org::omg::DynamicAny::DynAnyPackage {}
[[maybe_unused]] inline constexpr jPackage org_omg_DynamicAny_DynAnyPackage = org_omg_DynamicAny / "DynAnyPackage";
namespace org::omg::IOP {}
[[maybe_unused]] inline constexpr jPackage org_omg_IOP = org_omg / "IOP";
namespace org::omg::IOP::CodecFactoryPackage {}
[[maybe_unused]] inline constexpr jPackage org_omg_IOP_CodecFactoryPackage = org_omg_IOP / "CodecFactoryPackage";
namespace org::omg::IOP::CodecPackage {}
[[maybe_unused]] inline constexpr jPackage org_omg_IOP_CodecPackage = org_omg_IOP / "CodecPackage";
namespace org::omg::Messaging {}
[[maybe_unused]] inline constexpr jPackage org_omg_Messaging = org_omg / "Messaging";
namespace org::omg::PortableInterceptor {}
[[maybe_unused]] inline constexpr jPackage org_omg_PortableInterceptor = org_omg / "PortableInterceptor";
namespace org::omg::PortableInterceptor::ORBInitInfoPackage {}
[[maybe_unused]] inline constexpr jPackage org_omg_PortableInterceptor_ORBInitInfoPackage = org_omg_PortableInterceptor / "ORBInitInfoPackage";
namespace org::omg::PortableServer {}
[[maybe_unused]] inline constexpr jPackage org_omg_PortableServer = org_omg / "PortableServer";
namespace org::omg::PortableServer::CurrentPackage {}
[[maybe_unused]] inline constexpr jPackage org_omg_PortableServer_CurrentPackage = org_omg_PortableServer / "CurrentPackage";
namespace org::omg::PortableServer::POAManagerPackage {}
[[maybe_unused]] inline constexpr jPackage org_omg_PortableServer_POAManagerPackage = org_omg_PortableServer / "POAManagerPackage";
namespace org::omg::PortableServer::POAPackage {}
[[maybe_unused]] inline constexpr jPackage org_omg_PortableServer_POAPackage = org_omg_PortableServer / "POAPackage";
namespace org::omg::PortableServer::ServantLocatorPackage {}
[[maybe_unused]] inline constexpr jPackage org_omg_PortableServer_ServantLocatorPackage = org_omg_PortableServer / "ServantLocatorPackage";
namespace org::omg::PortableServer::portable {}
[[maybe_unused]] inline constexpr jPackage org_omg_PortableServer_portable = org_omg_PortableServer / "portable";
namespace org::omg::SendingContext {}
[[maybe_unused]] inline constexpr jPackage org_omg_SendingContext = org_omg / "SendingContext";
namespace org::omg::stub {}
[[maybe_unused]] inline constexpr jPackage org_omg_stub = org_omg / "stub";
namespace org::omg::stub::java {}
[[maybe_unused]] inline constexpr jPackage org_omg_stub_java = org_omg_stub / "java";
namespace org::omg::stub::java::rmi {}
[[maybe_unused]] inline constexpr jPackage org_omg_stub_java_rmi = org_omg_stub_java / "rmi";
namespace org::omg::stub::javax {}
[[maybe_unused]] inline constexpr jPackage org_omg_stub_javax = org_omg_stub / "javax";
namespace org::omg::stub::javax::management {}
[[maybe_unused]] inline constexpr jPackage org_omg_stub_javax_management = org_omg_stub_javax / "management";
namespace org::omg::stub::javax::management::remote {}
[[maybe_unused]] inline constexpr jPackage org_omg_stub_javax_management_remote = org_omg_stub_javax_management / "remote";
namespace org::omg::stub::javax::management::remote::rmi {}
[[maybe_unused]] inline constexpr jPackage org_omg_stub_javax_management_remote_rmi = org_omg_stub_javax_management_remote / "rmi";
namespace org::w3c {}
[[maybe_unused]] inline constexpr jPackage org_w3c = org_ / "w3c";
namespace org::w3c::dom {}
[[maybe_unused]] inline constexpr jPackage org_w3c_dom = org_w3c / "dom";
namespace org::w3c::dom::bootstrap {}
[[maybe_unused]] inline constexpr jPackage org_w3c_dom_bootstrap = org_w3c_dom / "bootstrap";
namespace org::w3c::dom::css {}
[[maybe_unused]] inline constexpr jPackage org_w3c_dom_css = org_w3c_dom / "css";
namespace org::w3c::dom::events {}
[[maybe_unused]] inline constexpr jPackage org_w3c_dom_events = org_w3c_dom / "events";
namespace org::w3c::dom::html {}
[[maybe_unused]] inline constexpr jPackage org_w3c_dom_html = org_w3c_dom / "html";
namespace org::w3c::dom::ls {}
[[maybe_unused]] inline constexpr jPackage org_w3c_dom_ls = org_w3c_dom / "ls";
namespace org::w3c::dom::ranges {}
[[maybe_unused]] inline constexpr jPackage org_w3c_dom_ranges = org_w3c_dom / "ranges";
namespace org::w3c::dom::stylesheets {}
[[maybe_unused]] inline constexpr jPackage org_w3c_dom_stylesheets = org_w3c_dom / "stylesheets";
namespace org::w3c::dom::traversal {}
[[maybe_unused]] inline constexpr jPackage org_w3c_dom_traversal = org_w3c_dom / "traversal";
namespace org::w3c::dom::views {}
[[maybe_unused]] inline constexpr jPackage org_w3c_dom_views = org_w3c_dom / "views";
namespace org::w3c::dom::xpath {}
[[maybe_unused]] inline constexpr jPackage org_w3c_dom_xpath = org_w3c_dom / "xpath";
namespace org::xml {}
[[maybe_unused]] inline constexpr jPackage org_xml = org_ / "xml";
namespace org::xml::sax {}
[[maybe_unused]] inline constexpr jPackage org_xml_sax = org_xml / "sax";
namespace org::xml::sax::ext {}
[[maybe_unused]] inline constexpr jPackage org_xml_sax_ext = org_xml_sax / "ext";
namespace org::xml::sax::helpers {}
[[maybe_unused]] inline constexpr jPackage org_xml_sax_helpers = org_xml_sax / "helpers";


#endif
