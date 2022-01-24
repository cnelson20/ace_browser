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
- important for whole program
- holds element's class, id, properties, etc.

#### struct key_value pair - used for \[bad\] pseudo-maps
- holds 2 char *'s

### Nonstandard Libraries
- ncurses ( <ncurses.h> ) 

## Instructions
### apt-get instructions:
- sudo apt install libncurses-dev

### how to use the project
- run make
- run ./console to view what you can do
  - ./console URL will open a web page and download it (do not include https://)
  - ./console -f FILENAME will open a local HTML file
  - ./console -s SITE PATH will open a web page and download it
  
- once you have a page open, use the up, down, left, and right arrow keys to move around the screen and the enter key to interact.
- input elements (for forms) have their colors inverted, and pressing enter will let you type until you press enter again
- submit type input elements work like buttons

### Issues (please be graceful)
- some pages dont render
- some pages seg fault (really big pages, specific elements)
- a lot of elements arent supported (the obvious like video audio img, but also other things
- in forms, only textarea and text/password inputs work (also required property is ignored)
