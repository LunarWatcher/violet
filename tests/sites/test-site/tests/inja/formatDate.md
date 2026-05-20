---
{
    "date": "2026-05-20T22:27:59+02:00"
}
---

Year: {{ formatDate(page.date, "%Y")}}

YMD: {{ formatDate(page.date, "%Y&%m&%d")}}

UTC+0: {{ formatDate(page.date, "%Y-%m-%d %H:%M:%OS%Oz") }}

UTC+2: {{ formatDate(page.date, "%Y-%m-%d %H:%M:%OS%Oz", "Europe/Oslo") }} (for this time of year)
