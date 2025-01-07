# GUI Chat Application
This project is a simple GUI-based chat application built using SDL2 to draw text to the screen, and low-level socket programming for networking. The primary goal of the project is to learn the basics of network programming by implementing a client-server chat system using system calls, inet, and netdb for network communication, without relying on higher-level socket libraries.

### Features:

  #### Text Entry via Terminal:
  Users enter messages directly in the terminal where the client program is executed.
  After sending, messages are displayed in a graphical chatbox rendered with SDL2.
        
  #### Message Broadcasting:
  The server receives messages from clients and broadcasts them to all connected clients, including the sender.
        
  #### Client-Side GUI:
  The client application visualizes the chat in a text box, with incoming messages dynamically updated in the GUI.

