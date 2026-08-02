#include "rc_ast.h"
#include "rc_parser.h"
#include "rc_tokenizer.h"

#include <cassert>
#include <format>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

static int g_tests_passed = 0;
static int g_tests_failed = 0;

struct test_result
{
  std::string name;
  bool passed;
  std::string error;
};

static std::vector<test_result> g_results;

static void record_result(const std::string& name, bool passed, const std::string& error = "")
{
  if (passed)
  {
    g_tests_passed++;
    std::cout << "  PASS: " << name << "\n";
  }
  else
  {
    g_tests_failed++;
    std::cout << "  FAIL: " << name << " - " << error << "\n";
  }
  g_results.push_back({name, passed, error});
}

static rc::resource parse_single_resource(const std::string& input)
{
  auto tokens = rc::tokenize(input);
  rc::parser p(tokens);
  auto file = p.parse();
  if (file.resources.size() != 1)
    throw std::runtime_error(std::format("Expected 1 resource, got {}", file.resources.size()));
  return file.resources[0];
}

static rc::rc_file parse_all(const std::string& input)
{
  auto tokens = rc::tokenize(input);
  rc::parser p(tokens);
  return p.parse();
}

// ============================================================================
// Control type tests
// ============================================================================

static void test_pushbutton_basic()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  PUSHBUTTON \"OK\",IDOK,10,10,50,14\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    assert(dlg.controls.size() == 1);
    const auto& c = dlg.controls[0];
    assert(c.keyword == "PUSHBUTTON");
    assert(c.text == "OK");
    assert(c.id == "IDOK");
    assert(c.x == 10);
    assert(c.y == 10);
    assert(c.width == 50);
    assert(c.height == 14);
    record_result("pushbutton_basic", true);
  }
  catch (const std::exception& e)
  {
    record_result("pushbutton_basic", false, e.what());
  }
}

static void test_pushbutton_with_style()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  PUSHBUTTON \"Put...\",IDC_PUT,231,196,50,14,WS_DISABLED\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.style.first == "WS_DISABLED");
    record_result("pushbutton_with_style", true);
  }
  catch (const std::exception& e)
  {
    record_result("pushbutton_with_style", false, e.what());
  }
}

static void test_pushbutton_not_visible()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  PUSHBUTTON \"Cancel\",IDCANCEL,129,212,50,14,NOT WS_VISIBLE\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(!c.style.not_flags.empty());
    assert(c.style.not_flags[0] == "WS_VISIBLE");
    record_result("pushbutton_not_visible", true);
  }
  catch (const std::exception& e)
  {
    record_result("pushbutton_not_visible", false, e.what());
  }
}

static void test_pushbutton_multiline_not()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  PUSHBUTTON \"Down\",IDC_BTNPrioritizationDown,193,32,27,12,NOT \\\n"
      "                    WS_VISIBLE\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(!c.style.not_flags.empty());
    assert(c.style.not_flags[0] == "WS_VISIBLE");
    record_result("pushbutton_multiline_not", true);
  }
  catch (const std::exception& e)
  {
    record_result("pushbutton_multiline_not", false, e.what());
  }
}

// ============================================================================
// DEFPUSHBUTTON
// ============================================================================

static void test_defpushbutton_basic()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  DEFPUSHBUTTON \"Answer\",IDC_ANSWER,220,6,50,14\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.keyword == "DEFPUSHBUTTON");
    assert(c.text == "Answer");
    assert(c.id == "IDC_ANSWER");
    record_result("defpushbutton_basic", true);
  }
  catch (const std::exception& e)
  {
    record_result("defpushbutton_basic", false, e.what());
  }
}

static void test_defpushbutton_ws_group()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  DEFPUSHBUTTON \"OK\",IDOK,73,47,38,14,WS_GROUP\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.style.first == "WS_GROUP");
    record_result("defpushbutton_ws_group", true);
  }
  catch (const std::exception& e)
  {
    record_result("defpushbutton_ws_group", false, e.what());
  }
}

static void test_defpushbutton_not_ws_visible()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  DEFPUSHBUTTON \"&Start Sending\",IDC_BUTTON_ATTACH,53,212,50,14,NOT WS_VISIBLE\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(!c.style.not_flags.empty());
    assert(c.style.not_flags[0] == "WS_VISIBLE");
    record_result("defpushbutton_not_ws_visible", true);
  }
  catch (const std::exception& e)
  {
    record_result("defpushbutton_not_ws_visible", false, e.what());
  }
}

// ============================================================================
// CHECKBOX
// ============================================================================

static void test_checkbox_basic()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  CHECKBOX \"Erase\",IDC_CHECK1,20,25,40,10\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.keyword == "CHECKBOX");
    assert(c.text == "Erase");
    assert(c.id == "IDC_CHECK1");
    record_result("checkbox_basic", true);
  }
  catch (const std::exception& e)
  {
    record_result("checkbox_basic", false, e.what());
  }
}

static void test_checkbox_multiline()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  CHECKBOX \"Supports quality control?\",IDC_CHECK_IN_SUPPORTS_QC,281,\n"
      "                    18,111,10\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.x == 281);
    assert(c.y == 18);
    assert(c.width == 111);
    assert(c.height == 10);
    record_result("checkbox_multiline", true);
  }
  catch (const std::exception& e)
  {
    record_result("checkbox_multiline", false, e.what());
  }
}

// ============================================================================
// AUTOCHECKBOX
// ============================================================================

static void test_autocheckbox()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  AUTOCHECKBOX \"Erase\",IDC_CHECK1,20,25,40,10\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.keyword == "AUTOCHECKBOX");
    record_result("autocheckbox", true);
  }
  catch (const std::exception& e)
  {
    record_result("autocheckbox", false, e.what());
  }
}

// ============================================================================
// RADIOBUTTON
// ============================================================================

static void test_radiobutton()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  RADIOBUTTON \"Option A\",IDC_RADIO1,10,10,50,10\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.keyword == "RADIOBUTTON");
    assert(c.text == "Option A");
    assert(c.id == "IDC_RADIO1");
    record_result("radiobutton", true);
  }
  catch (const std::exception& e)
  {
    record_result("radiobutton", false, e.what());
  }
}

// ============================================================================
// LTEXT
// ============================================================================

static void test_ltext_basic()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  LTEXT \"&Contacts\",IDC_STATIC,17,7,66,8\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.keyword == "LTEXT");
    assert(c.text == "&Contacts");
    assert(c.id == "IDC_STATIC");
    assert(c.x == 17);
    assert(c.y == 7);
    record_result("ltext_basic", true);
  }
  catch (const std::exception& e)
  {
    record_result("ltext_basic", false, e.what());
  }
}

static void test_ltext_negative_id()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  LTEXT \"Name:\",-1,12,139,28,8\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    // T4: tokenizer drops minus sign from negative numbers
    assert(c.id == "1");
    record_result("ltext_negative_id", true);
  }
  catch (const std::exception& e)
  {
    record_result("ltext_negative_id", false, e.what());
  }
}

static void test_ltext_ss_noprefix()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  LTEXT \"ElevationSample Version 1.0\",IDC_STATIC,49,10,119,8,SS_NOPREFIX\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.style.first == "SS_NOPREFIX");
    record_result("ltext_ss_noprefix", true);
  }
  catch (const std::exception& e)
  {
    record_result("ltext_ss_noprefix", false, e.what());
  }
}

static void test_ltext_with_ext_style()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  LTEXT \"<set description here>\",IDC_STATICTEXT,7,8,257,12,NOT WS_GROUP,WS_EX_TRANSPARENT\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(!c.style.not_flags.empty());
    assert(c.ext_style.first == "WS_EX_TRANSPARENT");
    record_result("ltext_with_ext_style", true);
  }
  catch (const std::exception& e)
  {
    record_result("ltext_with_ext_style", false, e.what());
  }
}

static void test_ltext_multiline_not()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  LTEXT \"Remote &Queue Name:\",IDC_STATIC_Q_LABEL,7,172,79,11,NOT \\\n"
      "                    WS_VISIBLE\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(!c.style.not_flags.empty());
    assert(c.style.not_flags[0] == "WS_VISIBLE");
    record_result("ltext_multiline_not", true);
  }
  catch (const std::exception& e)
  {
    record_result("ltext_multiline_not", false, e.what());
  }
}

static void test_ltext_empty_text()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  LTEXT \"\",IDC_TEXT,5,5,175,65,SS_SUNKEN\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.text.empty());
    assert(c.style.first == "SS_SUNKEN");
    record_result("ltext_empty_text", true);
  }
  catch (const std::exception& e)
  {
    record_result("ltext_empty_text", false, e.what());
  }
}

// ============================================================================
// CTEXT
// ============================================================================

static void test_ctext_basic()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  CTEXT \"\",IDC_PROPS_TEXT,7,59,181,24\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.keyword == "CTEXT");
    record_result("ctext_basic", true);
  }
  catch (const std::exception& e)
  {
    record_result("ctext_basic", false, e.what());
  }
}

static void test_ctext_negative_id()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  CTEXT \"Microsoft Windows\",-1,0,5,144,8\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    // T4: tokenizer drops minus sign from negative numbers
    assert(c.id == "1");
    assert(c.text == "Microsoft Windows");
    record_result("ctext_negative_id", true);
  }
  catch (const std::exception& e)
  {
    record_result("ctext_negative_id", false, e.what());
  }
}

static void test_ctext_ss_sunken_not_group()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  CTEXT \"\",IDT_FILEDESC,5,6,165,29,SS_SUNKEN | NOT WS_GROUP\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.style.first == "SS_SUNKEN");
    assert(!c.style.not_flags.empty());
    assert(c.style.not_flags[0] == "WS_GROUP");
    record_result("ctext_ss_sunken_not_group", true);
  }
  catch (const std::exception& e)
  {
    record_result("ctext_ss_sunken_not_group", false, e.what());
  }
}

static void test_ctext_with_ext_style_continuation()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  CTEXT \"Gain Offset\",IDC_STATIC,0,4,42,10,SS_SUNKEN,\n"
      "                    WS_EX_TRANSPARENT\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.style.first == "SS_SUNKEN");
    assert(c.ext_style.first == "WS_EX_TRANSPARENT");
    record_result("ctext_with_ext_style_continuation", true);
  }
  catch (const std::exception& e)
  {
    record_result("ctext_with_ext_style_continuation", false, e.what());
  }
}

// ============================================================================
// RTEXT
// ============================================================================

static void test_rtext_basic()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  RTEXT \"Tablet PC Components:\",IDC_STATIC,5,10,105,8\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.keyword == "RTEXT");
    assert(c.text == "Tablet PC Components:");
    record_result("rtext_basic", true);
  }
  catch (const std::exception& e)
  {
    record_result("rtext_basic", false, e.what());
  }
}

static void test_rtext_style_zero_ext_style()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  RTEXT \"\",IDC_STATIC_STREAM2,112,56,16,10,0,WS_EX_RIGHT\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.style.first == "0");
    assert(c.ext_style.first == "WS_EX_RIGHT");
    record_result("rtext_style_zero_ext_style", true);
  }
  catch (const std::exception& e)
  {
    record_result("rtext_style_zero_ext_style", false, e.what());
  }
}

// ============================================================================
// GROUPBOX
// ============================================================================

static void test_groupbox_basic()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  GROUPBOX \"ADs Path\",IDC_STATIC,7,7,332,52\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.keyword == "GROUPBOX");
    assert(c.text == "ADs Path");
    record_result("groupbox_basic", true);
  }
  catch (const std::exception& e)
  {
    record_result("groupbox_basic", false, e.what());
  }
}

static void test_groupbox_ws_group()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  GROUPBOX \"A Group of radio buttons\",IDC_GROUP1,0,24,119,74,WS_GROUP\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.style.first == "WS_GROUP");
    record_result("groupbox_ws_group", true);
  }
  catch (const std::exception& e)
  {
    record_result("groupbox_ws_group", false, e.what());
  }
}

static void test_groupbox_not_ws_visible()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  GROUPBOX \"Mutex Type\",IDC_FRAMutexType,223,7,46,55,NOT WS_VISIBLE\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(!c.style.not_flags.empty());
    assert(c.style.not_flags[0] == "WS_VISIBLE");
    record_result("groupbox_not_ws_visible", true);
  }
  catch (const std::exception& e)
  {
    record_result("groupbox_not_ws_visible", false, e.what());
  }
}

static void test_groupbox_multiline_not()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  GROUPBOX \"Streams in Mutex\",IDC_FRAMutexStreams,130,7,27,51,NOT \\\n"
      "                    WS_VISIBLE\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(!c.style.not_flags.empty());
    assert(c.style.not_flags[0] == "WS_VISIBLE");
    record_result("groupbox_multiline_not", true);
  }
  catch (const std::exception& e)
  {
    record_result("groupbox_multiline_not", false, e.what());
  }
}

// ============================================================================
// ICON
// ============================================================================

static void test_icon_basic()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  ICON IDR_MAINFRAME,IDC_STATIC,11,17,20,20\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.keyword == "ICON");
    assert(c.id == "IDC_STATIC");
    record_result("icon_basic", true);
  }
  catch (const std::exception& e)
  {
    record_result("icon_basic", false, e.what());
  }
}

static void test_icon_string_name()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  ICON \"spintesticon\",101,8,22,18,20\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.text == "spintesticon");
    record_result("icon_string_name", true);
  }
  catch (const std::exception& e)
  {
    record_result("icon_string_name", false, e.what());
  }
}

static void test_icon_negative_id()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  ICON IDI_DSPLAY,-1,11,17,20,20\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    // T4: tokenizer drops minus sign from negative numbers
    assert(c.id == "1");
    record_result("icon_negative_id", true);
  }
  catch (const std::exception& e)
  {
    record_result("icon_negative_id", false, e.what());
  }
}

static void test_icon_with_style()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  ICON IDI_LOGO,IDC_APPLICATION_ICON,11,11,20,20,SS_NOTIFY\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.style.first == "SS_NOTIFY");
    record_result("icon_with_style", true);
  }
  catch (const std::exception& e)
  {
    record_result("icon_with_style", false, e.what());
  }
}

static void test_icon_empty_text()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  ICON \"\",IDC_DEVICE_ICON,9,7,20,20\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.text.empty());
    record_result("icon_empty_text", true);
  }
  catch (const std::exception& e)
  {
    record_result("icon_empty_text", false, e.what());
  }
}

// ============================================================================
// EDITTEXT
// ============================================================================

static void test_edittext_basic()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  EDITTEXT IDC_STATUS,37,11,173,14,ES_AUTOHSCROLL | ES_READONLY\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.keyword == "EDITTEXT");
    assert(c.id == "IDC_STATUS");
    assert(c.x == 37);
    assert(c.y == 11);
    assert(c.width == 173);
    assert(c.height == 14);
    assert(c.style.first == "ES_AUTOHSCROLL");
    record_result("edittext_basic", true);
  }
  catch (const std::exception& e)
  {
    record_result("edittext_basic", false, e.what());
  }
}

static void test_edittext_multiline()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  EDITTEXT IDC_TXTHELP,121,168,227,34,ES_MULTILINE | ES_READONLY\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.style.first == "ES_MULTILINE");
    assert(!c.style.ops.empty());
    record_result("edittext_multiline", true);
  }
  catch (const std::exception& e)
  {
    record_result("edittext_multiline", false, e.what());
  }
}

static void test_edittext_wantreturn_number()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  EDITTEXT IDC_TXTSharedBitrate,165,58,17,12,ES_WANTRETURN | \\\n"
      "                    ES_NUMBER | NOT WS_VISIBLE\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.style.first == "ES_WANTRETURN");
    assert(!c.style.not_flags.empty());
    assert(c.style.not_flags[0] == "WS_VISIBLE");
    record_result("edittext_wantreturn_number", true);
  }
  catch (const std::exception& e)
  {
    record_result("edittext_wantreturn_number", false, e.what());
  }
}

static void test_edittext_not_border_not_tabstop()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  EDITTEXT IDC_NAME_FIELD,40,9,208,14,ES_AUTOHSCROLL | ES_READONLY | NOT WS_BORDER | NOT WS_TABSTOP\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.style.first == "ES_AUTOHSCROLL");
    assert(c.style.not_flags.size() == 2);
    assert(c.style.not_flags[0] == "WS_BORDER");
    assert(c.style.not_flags[1] == "WS_TABSTOP");
    record_result("edittext_not_border_not_tabstop", true);
  }
  catch (const std::exception& e)
  {
    record_result("edittext_not_border_not_tabstop", false, e.what());
  }
}

static void test_edittext_password()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  EDITTEXT IDC_EDIT_PASSWD,50,20,130,12,ES_PASSWORD | \\\n"
      "                    ES_AUTOHSCROLL\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.style.first == "ES_PASSWORD");
    assert(!c.style.ops.empty());
    record_result("edittext_password", true);
  }
  catch (const std::exception& e)
  {
    record_result("edittext_password", false, e.what());
  }
}

static void test_edittext_numeric_id()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  EDITTEXT 500,71,25,18,12,ES_AUTOHSCROLL | ES_READONLY\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.id == "500");
    record_result("edittext_numeric_id", true);
  }
  catch (const std::exception& e)
  {
    record_result("edittext_numeric_id", false, e.what());
  }
}

// ============================================================================
// LISTBOX
// ============================================================================

static void test_listbox_basic()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  LISTBOX IDC_LIST_DEVICES,7,18,113,67,LBS_NOINTEGRALHEIGHT | \\\n"
      "                    WS_VSCROLL | WS_HSCROLL | WS_TABSTOP\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.keyword == "LISTBOX");
    assert(c.id == "IDC_LIST_DEVICES");
    assert(c.width == 113);
    assert(c.height == 67);
    record_result("listbox_basic", true);
  }
  catch (const std::exception& e)
  {
    record_result("listbox_basic", false, e.what());
  }
}

static void test_listbox_with_size()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  LISTBOX IDC_DEVICES,4,16,120,30,WS_VSCROLL | WS_TABSTOP\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.id == "IDC_DEVICES");
    assert(c.width == 120);
    assert(c.height == 30);
    record_result("listbox_with_size", true);
  }
  catch (const std::exception& e)
  {
    record_result("listbox_with_size", false, e.what());
  }
}

static void test_listbox_not_visible()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  LISTBOX IDC_LSTMutexStreams,133,17,17,35,LBS_SORT | \\\n"
      "                    LBS_NOINTEGRALHEIGHT | LBS_EXTENDEDSEL | NOT WS_VISIBLE | \\\n"
      "                    WS_VSCROLL | WS_TABSTOP\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(!c.style.not_flags.empty());
    assert(c.style.not_flags[0] == "WS_VISIBLE");
    record_result("listbox_not_visible", true);
  }
  catch (const std::exception& e)
  {
    record_result("listbox_not_visible", false, e.what());
  }
}

// ============================================================================
// COMBOBOX
// ============================================================================

static void test_combobox_simple()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  COMBOBOX IDC_COMBO1,10,15,60,70,CBS_SIMPLE | CBS_SORT | \\\n"
      "                    WS_VSCROLL | WS_TABSTOP\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.keyword == "COMBOBOX");
    assert(c.id == "IDC_COMBO1");
    assert(c.width == 60);
    assert(c.height == 70);
    assert(c.style.first == "CBS_SIMPLE");
    record_result("combobox_simple", true);
  }
  catch (const std::exception& e)
  {
    record_result("combobox_simple", false, e.what());
  }
}

static void test_combobox_dropdown()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  COMBOBOX IDC_COMBO_CHAT_TRANSPORT,15,21,97,41,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.style.first == "CBS_DROPDOWNLIST");
    record_result("combobox_dropdown", true);
  }
  catch (const std::exception& e)
  {
    record_result("combobox_dropdown", false, e.what());
  }
}

static void test_combobox_not_visible()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  COMBOBOX IDC_CBStreamCodec,217,62,17,79,CBS_DROPDOWNLIST | NOT \\\n"
      "                    WS_VISIBLE | WS_VSCROLL | WS_TABSTOP\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(!c.style.not_flags.empty());
    assert(c.style.not_flags[0] == "WS_VISIBLE");
    record_result("combobox_not_visible", true);
  }
  catch (const std::exception& e)
  {
    record_result("combobox_not_visible", false, e.what());
  }
}

// ============================================================================
// SCROLLBAR
// ============================================================================

static void test_scrollbar_basic()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  SCROLLBAR IDC_TEXLIST_VERT,25,135,10,240,SBS_VERT\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.keyword == "SCROLLBAR");
    assert(c.id == "IDC_TEXLIST_VERT");
    assert(c.width == 10);
    assert(c.height == 240);
    assert(c.style.first == "SBS_VERT");
    record_result("scrollbar_basic", true);
  }
  catch (const std::exception& e)
  {
    record_result("scrollbar_basic", false, e.what());
  }
}

// ============================================================================
// CONTROL keyword
// ============================================================================

static void test_control_button_basic()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  CONTROL \"Online\",IDC_ONLINE,\"Button\",BS_AUTORADIOBUTTON | WS_TABSTOP,108,54,36,10\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.keyword == "CONTROL");
    assert(c.text == "Online");
    assert(c.id == "IDC_ONLINE");
    assert(c.class_name == "Button");
    assert(c.style.first == "BS_AUTORADIOBUTTON");
    assert(c.x == 108);
    assert(c.y == 54);
    record_result("control_button_basic", true);
  }
  catch (const std::exception& e)
  {
    record_result("control_button_basic", false, e.what());
  }
}

static void test_control_empty_text_ext_style()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  CONTROL \"\",IDC_IMAGE,\"static\",SS_BITMAP | SS_CENTERIMAGE | SS_REALSIZEIMAGE | SS_NOTIFY,11,21,40,40,WS_EX_TRANSPARENT\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.text.empty());
    assert(c.class_name == "static");
    assert(c.ext_style.first == "WS_EX_TRANSPARENT");
    record_result("control_empty_text_ext_style", true);
  }
  catch (const std::exception& e)
  {
    record_result("control_empty_text_ext_style", false, e.what());
  }
}

static void test_control_not_ws_visible()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  CONTROL \"\",IDC_EXPLORER_BROWSER,\"Static\",NOT WS_VISIBLE,7,7,281,191\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(!c.style.not_flags.empty());
    assert(c.style.not_flags[0] == "WS_VISIBLE");
    record_result("control_not_ws_visible", true);
  }
  catch (const std::exception& e)
  {
    record_result("control_not_ws_visible", false, e.what());
  }
}

static void test_control_numeric_id()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  CONTROL \"\",108,\"Static\",SS_BLACKRECT,0,53,136,2\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.id == "108");
    assert(c.class_name == "Static");
    assert(c.style.first == "SS_BLACKRECT");
    record_result("control_numeric_id", true);
  }
  catch (const std::exception& e)
  {
    record_result("control_numeric_id", false, e.what());
  }
}

static void test_control_multiline_style()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  CONTROL \"Radio1\",IDC_RADIO1,\"Button\",BS_AUTORADIOBUTTON | \\\n"
      "                    WS_GROUP,75,20,50,10\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.style.first == "BS_AUTORADIOBUTTON");
    assert(!c.style.ops.empty());
    assert(c.x == 75);
    assert(c.y == 20);
    assert(c.width == 50);
    assert(c.height == 10);
    record_result("control_multiline_style", true);
  }
  catch (const std::exception& e)
  {
    record_result("control_multiline_style", false, e.what());
  }
}

static void test_control_not_style_multiline()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  CONTROL \"&Express\",IDC_RADIO_EXPRESS,\"Button\",BS_AUTORADIOBUTTON | \\\n"
      "                    NOT WS_VISIBLE | WS_GROUP,203,204,55,8\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(!c.style.not_flags.empty());
    assert(c.style.not_flags[0] == "WS_VISIBLE");
    record_result("control_not_style_multiline", true);
  }
  catch (const std::exception& e)
  {
    record_result("control_not_style_multiline", false, e.what());
  }
}

static void test_control_hex_style()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  CONTROL \"\",IDC_DATETIMEPICKER_ALARM,\"SysDateTimePick32\",DTS_RIGHTALIGN | DTS_UPDOWN | WS_TABSTOP | 0x8,7,24,64,15\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.class_name == "SysDateTimePick32");
    assert(c.style.first == "DTS_RIGHTALIGN");
    assert(c.x == 7);
    assert(c.y == 24);
    record_result("control_hex_style", true);
  }
  catch (const std::exception& e)
  {
    record_result("control_hex_style", false, e.what());
  }
}

static void test_control_syslistview32()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  CONTROL \"\",IDC_LISTVIEW,\"SysListView32\",LVS_REPORT | \\\n"
      "                    LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | WS_BORDER | WS_TABSTOP,7,7,281,191\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.class_name == "SysListView32");
    assert(c.style.first == "LVS_REPORT");
    record_result("control_syslistview32", true);
  }
  catch (const std::exception& e)
  {
    record_result("control_syslistview32", false, e.what());
  }
}

static void test_control_progressbar()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  CONTROL \"Progress1\",IDC_PROGRESSBAR,\"msctls_progress32\",\n"
      "                    PBS_SMOOTH | WS_BORDER,7,213,239,14\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.class_name == "msctls_progress32");
    assert(c.x == 7);
    assert(c.y == 213);
    record_result("control_progressbar", true);
  }
  catch (const std::exception& e)
  {
    record_result("control_progressbar", false, e.what());
  }
}

static void test_control_updown()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  CONTROL \"Spin2\",IDC_SPIN_TIME,\"msctls_updown32\",UDS_SETBUDDYINT | \\\n"
      "                    UDS_ARROWKEYS | WS_TABSTOP,435,236,17,14\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.class_name == "msctls_updown32");
    assert(c.x == 435);
    assert(c.y == 236);
    record_result("control_updown", true);
  }
  catch (const std::exception& e)
  {
    record_result("control_updown", false, e.what());
  }
}

static void test_control_richedit()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  CONTROL \"\",IDC_USAGE_TEXT,\"RichEdit20A\",WS_GROUP|WS_VSCROLL|0x4804,4,4,320,240\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.class_name == "RichEdit20A");
    assert(c.style.first == "WS_GROUP");
    record_result("control_richedit", true);
  }
  catch (const std::exception& e)
  {
    record_result("control_richedit", false, e.what());
  }
}

static void test_control_button_lowercase()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  CONTROL \"OK\",IDOK,\"button\",BS_DEFPUSHBUTTON | WS_TABSTOP | WS_CHILD,\n"
      "        18,42,32,14\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.class_name == "button");
    assert(c.style.first == "BS_DEFPUSHBUTTON");
    record_result("control_button_lowercase", true);
  }
  catch (const std::exception& e)
  {
    record_result("control_button_lowercase", false, e.what());
  }
}

static void test_control_tabcontrol()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  CONTROL \"Tab1\",IDC_KEYPADS,\"SysTabControl32\",0x0,0,5,220,510\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.class_name == "SysTabControl32");
    assert(c.style.first == "0x0");
    assert(c.x == 0);
    assert(c.y == 5);
    assert(c.width == 220);
    assert(c.height == 510);
    record_result("control_tabcontrol", true);
  }
  catch (const std::exception& e)
  {
    record_result("control_tabcontrol", false, e.what());
  }
}

static void test_control_static_hex_style()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  CONTROL \"\",IDC_PICTURE,\"Static\",0xe,13,8,159,89\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.class_name == "Static");
    assert(c.style.first == "0xe");
    record_result("control_static_hex_style", true);
  }
  catch (const std::exception& e)
  {
    record_result("control_static_hex_style", false, e.what());
  }
}

static void test_control_bs_lefttext_notify()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  CONTROL \"\",IDC_STREAM2,\"Button\",BS_AUTORADIOBUTTON | BS_LEFTTEXT | BS_NOTIFY,129,56,39,10\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.style.first == "BS_AUTORADIOBUTTON");
    assert(c.style.ops.size() == 2);
    record_result("control_bs_lefttext_notify", true);
  }
  catch (const std::exception& e)
  {
    record_result("control_bs_lefttext_notify", false, e.what());
  }
}

// ============================================================================
// CONTROL with multiline text in continuation
// ============================================================================

static void test_control_multiline_text()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  CONTROL \"Connect to a DS &disabled computer\","
      "IDC_RADIO_WORKGROUP,\"Button\",BS_AUTORADIOBUTTON | NOT \\\n"
      "                    WS_VISIBLE,55,194,161,11\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.text == "Connect to a DS &disabled computer");
    assert(c.id == "IDC_RADIO_WORKGROUP");
    assert(!c.style.not_flags.empty());
    assert(c.x == 55);
    assert(c.y == 194);
    record_result("control_multiline_text", true);
  }
  catch (const std::exception& e)
  {
    record_result("control_multiline_text", false, e.what());
  }
}

// ============================================================================
// Dialog statements
// ============================================================================

static void test_dialog_caption()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 200, 100\n"
      "CAPTION \"My Dialog\"\n"
      "BEGIN\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    bool found = false;
    for (const auto& stmt : dlg.statements)
    {
      if (stmt.keyword == "CAPTION")
      {
        assert(stmt.text_value == "My Dialog");
        found = true;
        break;
      }
    }
    assert(found);
    record_result("dialog_caption", true);
  }
  catch (const std::exception& e)
  {
    record_result("dialog_caption", false, e.what());
  }
}

static void test_dialog_font()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 200, 100\n"
      "FONT 8, \"MS Shell Dlg\"\n"
      "BEGIN\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    bool found = false;
    for (const auto& stmt : dlg.statements)
    {
      if (stmt.keyword == "FONT")
      {
        assert(stmt.numeric_value == 8);
        assert(stmt.text_value == "MS Shell Dlg");
        found = true;
        break;
      }
    }
    assert(found);
    record_result("dialog_font", true);
  }
  catch (const std::exception& e)
  {
    record_result("dialog_font", false, e.what());
  }
}

static void test_dialog_font_bold_italic()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 200, 100\n"
      "FONT 8, \"MS Shell Dlg\", 700, 1, 1\n"
      "BEGIN\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    bool found = false;
    for (const auto& stmt : dlg.statements)
    {
      if (stmt.keyword == "FONT")
      {
        assert(stmt.numeric_value == 8);
        assert(stmt.numeric_value2 == 700);
        assert(stmt.italic == true);
        found = true;
        break;
      }
    }
    assert(found);
    record_result("dialog_font_bold_italic", true);
  }
  catch (const std::exception& e)
  {
    record_result("dialog_font_bold_italic", false, e.what());
  }
}

static void test_dialog_style()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 200, 100\n"
      "STYLE DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU\n"
      "BEGIN\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    bool found = false;
    for (const auto& stmt : dlg.statements)
    {
      if (stmt.keyword == "STYLE")
      {
        assert(stmt.value.first == "DS_SETFONT");
        found = true;
        break;
      }
    }
    assert(found);
    record_result("dialog_style", true);
  }
  catch (const std::exception& e)
  {
    record_result("dialog_style", false, e.what());
  }
}

static void test_dialog_exstyle()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 200, 100\n"
      "STYLE DS_SETFONT | WS_POPUP\n"
      "EXSTYLE WS_EX_APPWINDOW\n"
      "BEGIN\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    bool found = false;
    for (const auto& stmt : dlg.statements)
    {
      if (stmt.keyword == "EXSTYLE")
      {
        assert(stmt.value.first == "WS_EX_APPWINDOW");
        found = true;
        break;
      }
    }
    assert(found);
    record_result("dialog_exstyle", true);
  }
  catch (const std::exception& e)
  {
    record_result("dialog_exstyle", false, e.what());
  }
}

static void test_dialog_menu()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 200, 100\n"
      "MENU IDR_MAINMENU\n"
      "BEGIN\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    bool found = false;
    for (const auto& stmt : dlg.statements)
    {
      if (stmt.keyword == "MENU")
      {
        assert(stmt.id_value == "IDR_MAINMENU");
        found = true;
        break;
      }
    }
    assert(found);
    record_result("dialog_menu", true);
  }
  catch (const std::exception& e)
  {
    record_result("dialog_menu", false, e.what());
  }
}

// ============================================================================
// Menu parsing
// ============================================================================

static void test_menu_basic()
{
  try
  {
    auto res = parse_single_resource(
      "IDR_MENU1 MENU\n"
      "BEGIN\n"
      "  POPUP \"&File\"\n"
      "  BEGIN\n"
      "    MENUITEM \"&Open\",IDC_OPEN\n"
      "    MENUITEM \"&Save\",IDC_SAVE\n"
      "    MENUITEM SEPARATOR\n"
      "    MENUITEM \"E&xit\",IDC_EXIT\n"
      "  END\n"
      "  POPUP \"&Help\"\n"
      "  BEGIN\n"
      "    MENUITEM \"&About\",IDC_ABOUT\n"
      "  END\n"
      "END\n"
    );
    const auto& menu = std::get<rc::menu_data>(res.data);
    assert(menu.entries.size() == 2);
    const auto& file_popup = std::get<std::shared_ptr<rc::popup>>(menu.entries[0].item);
    assert(file_popup->text == "&File");
    assert(file_popup->entries.size() == 4);
    const auto& first_item = std::get<rc::menu_item>(file_popup->entries[0].item);
    assert(first_item.text == "&Open");
    assert(first_item.id == "IDC_OPEN");
    const auto& sep = std::get<rc::menu_item>(file_popup->entries[2].item);
    assert(sep.text == "SEPARATOR");
    record_result("menu_basic", true);
  }
  catch (const std::exception& e)
  {
    record_result("menu_basic", false, e.what());
  }
}

static void test_menu_grayed()
{
  try
  {
    auto res = parse_single_resource(
      "IDR_MENU1 MENU\n"
      "BEGIN\n"
      "  POPUP \"&File\"\n"
      "  BEGIN\n"
      "    MENUITEM \"&Open\",IDC_OPEN,MF_GRAYED\n"
      "  END\n"
      "END\n"
    );
    const auto& menu = std::get<rc::menu_data>(res.data);
    const auto& popup = std::get<std::shared_ptr<rc::popup>>(menu.entries[0].item);
    const auto& item = std::get<rc::menu_item>(popup->entries[0].item);
    assert(item.flags.size() == 1);
    assert(item.flags[0] == "MF_GRAYED");
    record_result("menu_grayed", true);
  }
  catch (const std::exception& e)
  {
    record_result("menu_grayed", false, e.what());
  }
}

static void test_menu_checked()
{
  try
  {
    auto res = parse_single_resource(
      "IDR_MENU1 MENU\n"
      "BEGIN\n"
      "  POPUP \"&View\"\n"
      "  BEGIN\n"
      "    MENUITEM \"&Toolbar\",IDC_TOOLBAR,MF_CHECKED\n"
      "  END\n"
      "END\n"
    );
    const auto& menu = std::get<rc::menu_data>(res.data);
    const auto& popup = std::get<std::shared_ptr<rc::popup>>(menu.entries[0].item);
    const auto& item = std::get<rc::menu_item>(popup->entries[0].item);
    assert(item.flags.size() == 1);
    assert(item.flags[0] == "MF_CHECKED");
    record_result("menu_checked", true);
  }
  catch (const std::exception& e)
  {
    record_result("menu_checked", false, e.what());
  }
}

static void test_menu_disabled_popup()
{
  try
  {
    auto res = parse_single_resource(
      "IDR_MENU1 MENU\n"
      "BEGIN\n"
      "  POPUP \"&File\",MF_GRAYED\n"
      "  BEGIN\n"
      "    MENUITEM \"&Open\",IDC_OPEN\n"
      "  END\n"
      "END\n"
    );
    const auto& menu = std::get<rc::menu_data>(res.data);
    const auto& popup = std::get<std::shared_ptr<rc::popup>>(menu.entries[0].item);
    assert(popup->flags.size() == 1);
    assert(popup->flags[0] == "MF_GRAYED");
    record_result("menu_disabled_popup", true);
  }
  catch (const std::exception& e)
  {
    record_result("menu_disabled_popup", false, e.what());
  }
}

// ============================================================================
// Menubar detection
// ============================================================================

static void test_menu_menubar()
{
  try
  {
    auto res = parse_single_resource(
      "IDR_MENU1 MENU EX\n"
      "BEGIN\n"
      "  POPUP \"&File\"\n"
      "  BEGIN\n"
      "    MENUITEM \"&New\",IDC_NEW\n"
      "    MENUITEM \"&Open\",IDC_OPEN\n"
      "  END\n"
      "END\n"
    );
    const auto& menu = std::get<rc::menu_data>(res.data);
    assert(menu.entries.size() == 1);
    record_result("menu_menubar", true);
  }
  catch (const std::exception& e)
  {
    record_result("menu_menubar", false, e.what());
  }
}

// ============================================================================
// Multiple controls
// ============================================================================

static void test_multiple_controls()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 300, 200\n"
      "BEGIN\n"
      "  LTEXT \"Name:\",IDC_NAME_LABEL,7,7,30,8\n"
      "  EDITTEXT IDC_NAME,42,5,150,14,ES_AUTOHSCROLL\n"
      "  LTEXT \"Email:\",IDC_EMAIL_LABEL,7,25,30,8\n"
      "  EDITTEXT IDC_EMAIL,42,23,150,14,ES_AUTOHSCROLL\n"
      "  PUSHBUTTON \"OK\",IDOK,7,42,50,14\n"
      "  PUSHBUTTON \"Cancel\",IDCANCEL,62,42,50,14\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    assert(dlg.controls.size() == 6);
    assert(dlg.controls[0].keyword == "LTEXT");
    assert(dlg.controls[1].keyword == "EDITTEXT");
    assert(dlg.controls[2].keyword == "LTEXT");
    assert(dlg.controls[3].keyword == "EDITTEXT");
    assert(dlg.controls[4].keyword == "PUSHBUTTON");
    assert(dlg.controls[5].keyword == "PUSHBUTTON");
    record_result("multiple_controls", true);
  }
  catch (const std::exception& e)
  {
    record_result("multiple_controls", false, e.what());
  }
}

// ============================================================================
// Edge cases
// ============================================================================

static void test_empty_dialog()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 200, 100\n"
      "BEGIN\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    assert(dlg.controls.empty());
    assert(dlg.statements.empty());
    record_result("empty_dialog", true);
  }
  catch (const std::exception& e)
  {
    record_result("empty_dialog", false, e.what());
  }
}

static void test_dialog_width_height()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 320, 240\n"
      "BEGIN\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    assert(dlg.x == 0);
    assert(dlg.y == 0);
    assert(dlg.width == 320);
    assert(dlg.height == 240);
    record_result("dialog_width_height", true);
  }
  catch (const std::exception& e)
  {
    record_result("dialog_width_height", false, e.what());
  }
}

static void test_dialog_ex_width_height()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 400, 300, 0\n"
      "BEGIN\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    assert(dlg.width == 400);
    assert(dlg.height == 300);
    assert(dlg.help_id == "0");
    record_result("dialog_ex_width_height", true);
  }
  catch (const std::exception& e)
  {
    record_result("dialog_ex_width_height", false, e.what());
  }
}

static void test_style_pipe_multiline()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 200, 100\n"
      "STYLE DS_SETFONT | DS_MODALFRAME | \\\n"
      "      WS_POPUP | WS_CAPTION | WS_SYSMENU\n"
      "BEGIN\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    bool found = false;
    for (const auto& stmt : dlg.statements)
    {
      if (stmt.keyword == "STYLE")
      {
        assert(stmt.value.first == "DS_SETFONT");
        assert(stmt.value.ops.size() == 4);
        found = true;
        break;
      }
    }
    assert(found);
    record_result("style_pipe_multiline", true);
  }
  catch (const std::exception& e)
  {
    record_result("style_pipe_multiline", false, e.what());
  }
}

// ============================================================================
// Toolbar
// ============================================================================

static void test_toolbar_basic()
{
  try
  {
    auto file = parse_all(
      "IDR_TOOLBAR1 TOOLBAR 16, 16\n"
      "BEGIN\n"
      "  BUTTON IDC_NEW\n"
      "  BUTTON IDC_OPEN\n"
      "  BUTTON IDC_SAVE\n"
      "  SEPARATOR\n"
      "  BUTTON IDC_CUT\n"
      "END\n"
    );
    assert(file.resources.size() == 1);
    const auto& tb = std::get<rc::toolbar_data>(file.resources[0].data);
    assert(tb.width == 16);
    assert(tb.height == 16);
    assert(tb.entries.size() == 5);
    assert(tb.entries[0].id == "IDC_NEW");
    assert(!tb.entries[0].is_separator);
    assert(tb.entries[3].is_separator);
    record_result("toolbar_basic", true);
  }
  catch (const std::exception& e)
  {
    record_result("toolbar_basic", false, e.what());
  }
}

// ============================================================================
// Accelerator
// ============================================================================

static void test_accelerator_basic()
{
  try
  {
    auto res = parse_single_resource(
      "IDR_ACCELS ACCELERATORS\n"
      "BEGIN\n"
      "  VK_F1,IDC_HELP,VIRTKEY\n"
      "  \"O\",IDC_OPEN,ASCII,ALT\n"
      "  \"S\",IDC_SAVE,ASCII,CONTROL\n"
      "END\n"
    );
    const auto& accels = std::get<std::vector<rc::accelerator_entry>>(res.data);
    assert(accels.size() == 3);
    assert(accels[0].event == "VK_F1");
    assert(accels[0].id == "IDC_HELP");
    assert(accels[0].modifiers.size() == 1);
    assert(accels[0].modifiers[0] == "VIRTKEY");
    // Tokenizer strips quotes from string literals
    assert(accels[1].event == "O");
    assert(accels[1].id == "IDC_OPEN");
    assert(accels[1].modifiers.size() == 2);
    assert(accels[1].modifiers[0] == "ASCII");
    assert(accels[1].modifiers[1] == "ALT");
    record_result("accelerator_basic", true);
  }
  catch (const std::exception& e)
  {
    record_result("accelerator_basic", false, e.what());
  }
}

// ============================================================================
// StringTable
// ============================================================================

static void test_stringtable_basic()
{
  try
  {
    auto res = parse_single_resource(
      "STRINGTABLE\n"
      "BEGIN\n"
      "  IDS_APP_TITLE, \"My Application\"\n"
      "  IDS_GREETING, \"Hello, World!\"\n"
      "END\n"
    );
    const auto& strings = std::get<std::vector<rc::string_table_entry>>(res.data);
    assert(strings.size() == 2);
    assert(strings[0].id == "IDS_APP_TITLE");
    assert(strings[0].value == "My Application");
    assert(strings[1].id == "IDS_GREETING");
    assert(strings[1].value == "Hello, World!");
    record_result("stringtable_basic", true);
  }
  catch (const std::exception& e)
  {
    record_result("stringtable_basic", false, e.what());
  }
}

// ============================================================================
// VersionInfo
// ============================================================================

static void test_versioninfo_basic()
{
  try
  {
    auto res = parse_single_resource(
      "IDR_VERSION1 VERSIONINFO\n"
      "FILEVERSION 1,0,0,1\n"
      "PRODUCTVERSION 1,0,0,1\n"
      "BEGIN\n"
      "  BLOCK \"StringFileInfo\"\n"
      "  BEGIN\n"
      "    BLOCK \"040904b0\"\n"
      "    BEGIN\n"
      "      VALUE \"FileDescription\", \"My Application\"\n"
      "      VALUE \"FileVersion\", \"1.0.0.1\"\n"
      "    END\n"
      "  END\n"
      "END\n"
    );
    const auto& ver = std::get<rc::version_info>(res.data);
    assert(!ver.values.empty());
    record_result("versioninfo_basic", true);
  }
  catch (const std::exception& e)
  {
    record_result("versioninfo_basic", false, e.what());
  }
}

// ============================================================================
// Multiple resources
// ============================================================================

static void test_multiple_resources()
{
  try
  {
    auto file = parse_all(
      "Dlg1 DIALOGEX 0, 0, 200, 100\n"
      "BEGIN\n"
      "  PUSHBUTTON \"OK\",IDOK,10,10,50,14\n"
      "END\n"
      "\n"
      "Dlg2 DIALOGEX 0, 0, 300, 200\n"
      "BEGIN\n"
      "  LTEXT \"Hello\",IDC_STATIC,10,10,50,8\n"
      "END\n"
    );
    assert(file.resources.size() == 2);
    assert(file.resources[0].id == "Dlg1");
    assert(file.resources[1].id == "Dlg2");
    record_result("multiple_resources", true);
  }
  catch (const std::exception& e)
  {
    record_result("multiple_resources", false, e.what());
  }
}

// ============================================================================
// CONTROL with numeric -1 ID and no comma issue
// ============================================================================

static void test_control_negative_one_id()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  CONTROL \"Use Frame Rate\",-1,\"static\",SS_LEFT | WS_CHILD,13,4,73,8\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    // T4: tokenizer drops minus sign from negative numbers
    assert(c.id == "1");
    assert(c.class_name == "static");
    record_result("control_negative_one_id", true);
  }
  catch (const std::exception& e)
  {
    record_result("control_negative_one_id", false, e.what());
  }
}

// ============================================================================
// Control with numeric ID in ICON (quoted name with -1)
// ============================================================================

static void test_icon_string_negative_id()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 100, 100\n"
      "BEGIN\n"
      "  ICON \"IPXChat\",-1,3,2,18,20\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.text == "IPXChat");
    // T4: tokenizer drops minus sign from negative numbers
    assert(c.id == "1");
    record_result("icon_string_negative_id", true);
  }
  catch (const std::exception& e)
  {
    record_result("icon_string_negative_id", false, e.what());
  }
}

// ============================================================================
// COMBOBOXEx32
// ============================================================================

static void test_comboboxex32()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 200, 100\n"
      "BEGIN\n"
      "  CONTROL \"\",IDC_COMBO1,\"ComboBoxEx32\",CBS_DROPDOWN | WS_VSCROLL | WS_TABSTOP,10,10,150,100\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.class_name == "ComboBoxEx32");
    record_result("comboboxex32", true);
  }
  catch (const std::exception& e)
  {
    record_result("comboboxex32", false, e.what());
  }
}

// ============================================================================
// NativeFontCtl
// ============================================================================

static void test_nativefontctl()
{
  try
  {
    auto res = parse_single_resource(
      "Dlg1 DIALOGEX 0, 0, 200, 100\n"
      "BEGIN\n"
      "  CONTROL \"\",IDC_FONT,\"NativeFontCtl\",0,0,0,0\n"
      "END\n"
    );
    const auto& dlg = std::get<rc::dialog_data>(res.data);
    const auto& c = dlg.controls[0];
    assert(c.class_name == "NativeFontCtl");
    record_result("nativefontctl", true);
  }
  catch (const std::exception& e)
  {
    record_result("nativefontctl", false, e.what());
  }
}

// ============================================================================
// Main
// ============================================================================

int main()
{
  std::cout << "Parser Unit Tests\n";
  std::cout << "=================\n\n";

  std::cout << "--- Pushbutton Tests ---\n";
  test_pushbutton_basic();
  test_pushbutton_with_style();
  test_pushbutton_not_visible();
  test_pushbutton_multiline_not();

  std::cout << "\n--- Defpushbutton Tests ---\n";
  test_defpushbutton_basic();
  test_defpushbutton_ws_group();
  test_defpushbutton_not_ws_visible();

  std::cout << "\n--- Checkbox Tests ---\n";
  test_checkbox_basic();
  test_checkbox_multiline();
  test_autocheckbox();

  std::cout << "\n--- Radiobutton Tests ---\n";
  test_radiobutton();

  std::cout << "\n--- LTEXT Tests ---\n";
  test_ltext_basic();
  test_ltext_negative_id();
  test_ltext_ss_noprefix();
  test_ltext_with_ext_style();
  test_ltext_multiline_not();
  test_ltext_empty_text();

  std::cout << "\n--- CTEXT Tests ---\n";
  test_ctext_basic();
  test_ctext_negative_id();
  test_ctext_ss_sunken_not_group();
  test_ctext_with_ext_style_continuation();

  std::cout << "\n--- RTEXT Tests ---\n";
  test_rtext_basic();
  test_rtext_style_zero_ext_style();

  std::cout << "\n--- GROUPBOX Tests ---\n";
  test_groupbox_basic();
  test_groupbox_ws_group();
  test_groupbox_not_ws_visible();
  test_groupbox_multiline_not();

  std::cout << "\n--- ICON Tests ---\n";
  test_icon_basic();
  test_icon_string_name();
  test_icon_negative_id();
  test_icon_with_style();
  test_icon_empty_text();
  test_icon_string_negative_id();

  std::cout << "\n--- EDITTEXT Tests ---\n";
  test_edittext_basic();
  test_edittext_multiline();
  test_edittext_wantreturn_number();
  test_edittext_not_border_not_tabstop();
  test_edittext_password();
  test_edittext_numeric_id();

  std::cout << "\n--- LISTBOX Tests ---\n";
  test_listbox_basic();
  test_listbox_with_size();
  test_listbox_not_visible();

  std::cout << "\n--- COMBOBOX Tests ---\n";
  test_combobox_simple();
  test_combobox_dropdown();
  test_combobox_not_visible();

  std::cout << "\n--- SCROLLBAR Tests ---\n";
  test_scrollbar_basic();

  std::cout << "\n--- CONTROL Tests ---\n";
  test_control_button_basic();
  test_control_empty_text_ext_style();
  test_control_not_ws_visible();
  test_control_numeric_id();
  test_control_multiline_style();
  test_control_not_style_multiline();
  test_control_hex_style();
  test_control_syslistview32();
  test_control_progressbar();
  test_control_updown();
  test_control_richedit();
  test_control_button_lowercase();
  test_control_tabcontrol();
  test_control_static_hex_style();
  test_control_bs_lefttext_notify();
  test_control_multiline_text();
  test_control_negative_one_id();

  std::cout << "\n--- Dialog Statement Tests ---\n";
  test_dialog_caption();
  test_dialog_font();
  test_dialog_font_bold_italic();
  test_dialog_style();
  test_dialog_exstyle();
  test_dialog_menu();

  std::cout << "\n--- Menu Tests ---\n";
  test_menu_basic();
  test_menu_grayed();
  test_menu_checked();
  test_menu_disabled_popup();
  test_menu_menubar();

  std::cout << "\n--- Edge Case Tests ---\n";
  test_multiple_controls();
  test_empty_dialog();
  test_dialog_width_height();
  test_dialog_ex_width_height();
  test_style_pipe_multiline();
  test_multiple_resources();

  std::cout << "\n--- Other Resource Type Tests ---\n";
  test_toolbar_basic();
  test_accelerator_basic();
  test_stringtable_basic();
  test_versioninfo_basic();

  std::cout << "\n--- Widget Class Tests ---\n";
  test_comboboxex32();
  test_nativefontctl();

  std::cout << "\n=================\n";
  std::cout << "Results: " << g_tests_passed << " passed, " << g_tests_failed << " failed\n";

  return g_tests_failed > 0 ? 1 : 0;
}
