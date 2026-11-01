{# expected; 3 4 1 5 2 #}
{% set byCreated = listPagesPaginated("/datedContent/", violet.sort.by_creation_date) %}

{# expected; 5 1 2 4 3 #}
{% set byModified = listPagesPaginated("/datedContent/", violet.sort.by_last_modified_date) %}

{# expected; 1 2 3 4 5 #}
{% set byTitle = listPagesPaginated("/datedContent/", violet.sort.by_title) %}

\[\[violet-test(byTitle):||{{ byTitle }}||\]\]

\[\[violet-test(byModified):||{{ byModified }}||\]\]

\[\[violet-test(byCreated):||{{ byCreated }}||\]\]
