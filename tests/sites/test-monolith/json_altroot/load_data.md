{% set absData = loadJson("/json/data.json") %}

\[\[violet-test(absData):{{ absData }}\]\]

\[\[violet-test(hasTransRights):{{ absData.trans_rights_are_human_rights }}]]
