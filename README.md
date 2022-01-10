# ace_browser

Cameron Nelson, Eric Guo, Adnan Quayyum
Pd 5

In terminal text-based web browser.

Systems Elements:
network - obvious
files - browser can open files locally and uses them store downloaded webpages temporarily.
metadata as well - stat

Data Structures:
struct html_element - holds data about html element
- tag name, properties, innerHTML (as js calls it), parent, children (& number of)

struct element_properties - holds an element's properties
- important for css
- holds element's class, id, style, etc.

