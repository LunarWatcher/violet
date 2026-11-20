---
---
/*
This CSS file is a part of violet's _default theme, an AI slop-free static site generator under the MIT license.
https://codeberg.org/LunarWatcher/violet/src/branch/master/LICENSE
Built with violet {{ violet.version }}
 */

hljs.addPlugin({
    "after:highlightElement": ({ el, result, _ }) =>  {
        // This is used to display the language, but it might be a better idea to add a proper element.
        el.setAttribute("data-language", result.language)
    },
});

hljs.highlightAll();

document.querySelectorAll(
    "pre > code"
).forEach(el => {
    el.innerHTML = el.innerHTML
        .replace(
            /^(.*)$/gm,
            '<span class="hljs-line-shim">$1</span>'
        );
})

