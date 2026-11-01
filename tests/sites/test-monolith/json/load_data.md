{% set relData = loadJson("data.json") %}
{% set absData = loadJson("/json/data.json") %}

\[\[violet-test(equality):{{ relData == absData }}\]\]

\[\[violet-test(relData):{{ relData }}\]\]

\[\[violet-test(hasTransRights):{{ relData.trans_rights_are_human_rights }}]]
