# DLGINIT resource

Defines control initialization data for a dialog box. Windows uses this custom binary resource block to feed initial settings to controls right before the dialog is displayed.

``` syntax
dialogID DLGINIT
BEGIN
    controlID, messageID, dataByteCount, reserved
    data-words . . .
    0
END
```

## Parameters

<dl> <dt>

<span id="dialogID"></span>*dialogID*
</dt> <dd>

The resource identifier of the dialog box to which these initialization instructions apply.

</dd> <dt>

<span id="controlID"></span>*controlID*
</dt> <dd>

The resource identifier for the specific control inside the dialog being initialized (for example, a combo box or list box).

</dd> <dt>

<span id="messageID"></span>*messageID*
</dt> <dd>

A Win32 message identifier specifying the action to perform on the control. Common values include:

| Value  | Message     | Description                           |
|--------|-------------|---------------------------------------|
| 0x0401 | LB_ADDSTRING | Add a string to a list box           |
| 0x0403 | CB_ADDSTRING | Add a string to a combo box          |
| 0x0402 | LB_DIR       | Add a directory listing to a list box |
| 0x0404 | CB_DIR       | Add a directory listing to a combo box |

</dd> <dt>

<span id="dataByteCount"></span>*dataByteCount*
</dt> <dd>

The total size of the data payload in bytes, including any null-terminator.

</dd> <dt>

<span id="reserved"></span>*reserved*
</dt> <dd>

Reserved field. Set to 0 for standard string additions.

</dd> </dl>

## Remarks

The Resource Compiler encodes ANSI/Unicode string payloads as sequence words (16-bit integers in little-endian byte order). Odd-length strings are padded with a null byte to maintain word alignment.

A final **0** (zero) signals the end of the DLGINIT data stream for a dialog block.

## Examples

The following example initializes a combo box control with three string entries:

``` syntax
IDD_OBJECTKEYPAD DLGINIT
BEGIN
    IDC_COORDSYS_SELECT, 0x403, 6, 0
0x6f4c, 0x6163, 0x006c,
    IDC_COORDSYS_SELECT, 0x403, 7, 0
0x6956, 0x7765, 0x7265, "\000"
    IDC_COORDSYS_SELECT, 0x403, 6, 0
0x6f57, 0x6c72, 0x0064,
    0
END
```

This adds the strings "Local", "Viewer", and "World" to the combo box identified by `IDC_COORDSYS_SELECT`. Each entry uses message 0x0403 (CB\_ADDSTRING). The hexadecimal data words decode as follows:

| Entry | Byte Count | Hex Words             | Decoded String |
|-------|-----------|----------------------|----------------|
| 1     | 6         | 0x6f4c, 0x6163, 0x006c | "Local"       |
| 2     | 7         | 0x6956, 0x7765, 0x7265, "\000" | "Viewer" |
| 3     | 6         | 0x6f57, 0x6c72, 0x0064 | "World"       |

## See also

<dl> <dt>

[**DIALOG**](dialog-resource.md)
</dt> <dt>

[**DIALOGEX**](dialogex-resource.md)
</dt> </dl>
