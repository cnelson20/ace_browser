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

#### How it will work:
curl handles downloading html files
curl, via flags --cookie-jar and --cookie, can write / read cookies from a site.
--output sends the downloaded page to a file, --silent suppresses any output to stdio (good) 
--dump-header stores the http headers from a server's response to a file as well.

For example,
```
curl site.com/login --output page.html --cookie-jar cookies.txt -F "username=username" -F "password=password"
```
would allow the client to log in while storing necessary cookies.
To then access, for example, an account settings page (or anywhere else that would require a login) 
```
curl site.com/account --output page.html --cookie cookies.txt 
```
could be used (i tested this with my softdev project as the site, it worked)

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

## Timeline
 
1/18: Basic rendering of html page (without styling)  
1/21: Putting output into ncurses window, let user scoll around page.  
1/24: Styling, html form support.  
