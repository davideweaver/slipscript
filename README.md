# SlipScript

A server-side scripting language for embedding dynamic content in HTML pages. Created for Windows 95/NT web servers in the mid-1990s.

## Overview

SlipScript enables authors to create dynamic web content by embedding scripts directly into HTML files. When a page is requested, the script executes before the HTML is returned to the browser. This allows CGI-style scripting without separate CGI programs.

**Historical Context**: SlipScript was developed in 1994-1995 during the early days of dynamic web content, before technologies like PHP, ASP, and JSP became mainstream. It represents an early approach to server-side scripting on Windows platforms.

## Interpreter

SlipScript is built on **BOB** (a tiny object-oriented language) by David Michael Betz, published in Dr. Dobb's Journal, September 1994. BOB uses a bytecode virtual machine with stack-based execution, compiling scripts once to bytecode and interpreting at runtime for efficiency.

**Documentation**: See [`docs/bob/`](docs/bob/) for detailed architecture documentation and complete source code.
**Reference**: [Bob: A Tiny Object-Oriented Language](https://jacobfilipp.com/DrDobbs/articles/DDJ/1994/9415/9415e/9415e.htm) - Dr. Dobb's Journal, "Alternative Programming Languages" volume

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
├── docs/             # Documentation
│   ├── website/      # Original website (1994-95) with screenshots
│   │   ├── README.md      # Website documentation
│   │   ├── index.html     # Frameset entry point
│   │   ├── ss_guide/      # Installation, tutorial, API guides
│   │   └── screenshots/   # Website screenshots (overview, install, tutorial, APIs)
│   └── bob/          # BOB interpreter documentation and source
│       ├── README.md    # Technical architecture guide
│       ├── ARTICLE.md   # DDJ article citation
│       └── bob.asc      # Complete source code (669 lines)
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

The project includes comprehensive documentation:

**Original Website** ([`docs/website/`](docs/website/)):
- Classic 1994-95 frameset design with screenshots
- Installation guides (FastTrack, Enterprise, IIS)
- Interactive tutorials and examples
- Complete API reference

**BOB Interpreter** ([`docs/bob/`](docs/bob/)):
- Technical architecture documentation
- Complete source code from DDJ article
- Virtual machine design and bytecode details

```bash
# View original website
open docs/website/index.html

# Or serve locally
cd docs/website && python3 -m http.server 8000
```

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

## Authors

**SlipScript Implementation**: David Weaver (1994-1995)
**BOB Interpreter**: David Michael Betz (1994)

## Archive Notice

This is historical software from the mid-1990s Windows web server era. It is preserved for educational and historical purposes. The code reflects the technology constraints and approaches of that period and is not recommended for production use on modern systems.
