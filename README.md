# SlipScript

A server-side scripting language for embedding dynamic content in HTML pages. Created for Windows 95/NT web servers in the mid-1990s.

## Overview

SlipScript enables authors to create dynamic web content by embedding scripts directly into HTML files. When a page is requested, the script executes before the HTML is returned to the browser. This allows CGI-style scripting without separate CGI programs.

**Historical Context**: SlipScript was developed in 1994-1995 during the early days of dynamic web content, before technologies like PHP, ASP, and JSP became mainstream. It represents an early approach to server-side scripting on Windows platforms.

## Quick Example

```html
<HTML>
<@
    // Get and display the user agent
    val = getValue( "HTTP_USER_AGENT" );
    write( val );
@>
</HTML>
```

Scripts are embedded between `<@ ... @>` tags and have access to HTTP server variables and CGI functions.

## System Requirements

**Original Requirements** (Windows 95/NT era):
- Windows 95 or Windows NT 3.51+
- 32-bit ODBC drivers
- CGI 1.1-compatible web server
- Server must support file extension associations

**Note**: This is legacy software targeting mid-1990s Windows platforms. It may require significant adaptation for modern systems.

## Project Structure

```
slipscript/
├── bin/              # Compiled DLL (slipscript.dll)
├── docs/             # HTML documentation with frameset interface
│   ├── index.html    # Documentation entry point
│   └── ss_guide/     # Installation and API guides
├── examples/         # Sample .SLIP files
│   ├── VARIABLES.SLIP
│   ├── MAILTEST.SLIP
│   ├── SERVICES.SLIP
│   └── calendar/
├── include/          # Header files
├── is_slip/          # Core implementation
├── lib/              # Library files
└── src/              # Source code
    ├── INTERP/       # Interpreter
    ├── CGIHTML/      # CGI/HTML handling
    ├── NET_TOOLS/    # Network utilities
    └── UTILITY/      # Helper functions
```

## Features

- **Embedded Scripting**: Write server-side code directly in HTML
- **CGI Integration**: Access HTTP variables and form data
- **ODBC Support**: Database connectivity via ODBC
- **File Extension Association**: `.SLIP` files automatically processed
- **Windows Native**: Implemented as Windows DLL

## Documentation

The project includes comprehensive HTML documentation:

```bash
# Open the documentation
open docs/index.html
```

The documentation uses a frameset interface with:
- Installation guides
- API reference
- Language tutorials
- Example code

## Building

**Original Build Environment**:
- Microsoft Visual C++ (Visual Studio 6.0 era)
- Project files: `slipscript.dsp` (DevStudio Project)
- Workspace: `slipscript.dsw` (DevStudio Workspace)

The compiled output is a Windows DLL (`slipscript.dll`) that acts as a CGI executable.

## Historical Significance

SlipScript represents an important piece of web development history:

- **Early SSI Alternative**: Pre-dates widespread PHP adoption
- **Windows CGI Era**: Designed for Windows-based web hosting
- **1994-1995 Technology**: Contemporary with early Apache, IIS, and Netscape servers
- **Embedded Syntax**: Similar approach to later technologies (ASP, JSP, PHP)

## Modern Context

This project is preserved as a historical artifact. For modern server-side scripting, consider:

- **PHP**: Industry standard for embedded scripting
- **Node.js**: JavaScript-based server-side development
- **Python/Django**: Full-featured web framework
- **ASP.NET Core**: Modern Microsoft web platform

## License

See repository for license information.

## Author

David Weaver (1994-1995)

## Archive Notice

This is historical software from the mid-1990s Windows web server era. It is preserved for educational and historical purposes. The code reflects the technology constraints and approaches of that period and is not recommended for production use on modern systems.
