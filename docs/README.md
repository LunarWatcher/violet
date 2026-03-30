---
{
    "title": "How to read the documentation",
    "type": "page_list"
}
---

# How to read the documentation

This page contains general information about the documentation itself. Failure to read this will likely result in the documentation being misunderstood. Spending a few minutes to read it now will likely save you a lot of time down the line.

## Violet uses json, but the documentation uses jsonc

Json is not the best format for configuration, largely because of the lack of comments, but this limitation isn't really as bad for these kinds of applications. The technical reason for why json is used anyway is that there are very few alternative _parser implementations_ in C++. There is a YAML library, but it has some severe problems, and as of at least 2025-09-27, it has been enshittified with slop. There are far more JSON implementations, however, and through [inja](https://github.com/pantor/inja/), we're already using a JSON library (nlohmann/json; currently (to my knowledge) slop-free). Therefore, JSON _is_ the more convenient option.

Comments are nice, sure, but they can always be substituted by a decent commit history and decent documentation in the tool. However, the one exception to this is in said decent documentation, where the JSON may need to be directly commented.

As a consequence, while violet's config files are comment-free JSON, the documentation uses jsonc to be able to explain things. If you copy-paste config, you must remove the comments before running, or violet will error out.

