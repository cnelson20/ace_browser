# ace_browser

#### Cameron Nelson, Eric Guo, Adnan Quayyum  
#### Pd 5

## About
### Synopsis
In terminal text-based web browser.

### UI
The user will be able to move around / scroll the webpage using terminal cursor and the arrow keys (we'll probably use ncurses).    
They can navigate around the web by 'clicking' (actually by pressing enter) on links or by pressing buttons on html forms.  
The user can also just type the url of the page they want to access.

### Responsibilities:
- Cameron: HTML && CSS Parsing
- Adnan: Terminal UI
- Eric: Downloading Webpages

### Data Structures:
#### struct html_element - holds data about html element
- tag name, properties, innerHTML (as js calls it), parent, children (& number of)

#### struct element_properties - holds an element's properties
- important for css
- holds element's class, id, style, etc.

