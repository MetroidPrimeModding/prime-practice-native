#ifndef PRIME_PRACTICE_JSKERNEL_HPP
#define PRIME_PRACTICE_JSKERNEL_HPP

const char* js_kernel = ""
    "if (typeof global === 'undefined') {\n"
    "    (function () {\n"
    "        var global = new Function('return this;')();\n"
    "        Object.defineProperty(global, 'global', {\n"
    "            value: global,\n"
    "            writable: true,\n"
    "            enumerable: false,\n"
    "            configurable: true\n"
    "        });\n"
    "    })();\n"
    "}\n"
    "try {\n"
      "OSReport('Kernel loading mod.js');\n"
      "nativeRequire('/mod.js')();\n"
    "} catch (error) {\n"
      "OSReport('Error: ' + error + ' ' + error.stack);\n"
      "global.onFrame = function onframe() {\n"
        "setTextColor(1, 0, 0, 1);\n"
        "drawText(error + '\\n' + error.stack, 5, 5);\n"
      "};\n"
      "OSReport('Set new frame handler to render error');"
    "}\n";

#endif //PRIME_PRACTICE_JSKERNEL_HPP
