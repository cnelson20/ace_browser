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


### How it works:
curl handles downloading html files
curl, via flags --cookie-jar and --cookie, can write / read cookies from a site.
--output sends the downloaded page to a file, --silent suppresses any output to stdio (good) 
--dump-header stores the http headers from a server's response to a file as well.

For example,
```
curl site.com/login --output page.html --cookie-jar cookies.txt -F "username=username" -F "password=password"
```
allows the client to log in while storing necessary cookies.
To then access, for example, an account settings page (or anywhere else that would require a login) 
` curl site.com/account --output page.html --cookie cookies.txt ` is usable.
