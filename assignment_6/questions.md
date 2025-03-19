1. How does the remote client determine when a 
command's output is fully received from the 
server, and what techniques can be used to handle 
partial reads or ensure complete message 
transmission?

Answer: The client waits until it sees a special end
The client waits until it sees a special end marker (in this case, an EOF character (0x04)) that the server sends after finishing the output. Because TCP may split the data into multiple packets, the client must continuously read until it detects that marker.
Other common techniques include:

(i)Length Prefixing: TThe server first sends the length of the message, so the receiver knows exactly how many bytes to expect.


(ii)Delimiters: A unique sequence of characters can be used to indicate the end of a message (e.g., \n, \0, EOF).


(iii)Loop Until Full Data is Received: The client must keep reading in a loop and concatenate received data until the complete message is detected.


2. TCP is a reliable stream protocol rather than 
message-oriented. Since TCP does not preserve 
message boundaries, how should a networked shell 
protocol define and detect the beginning and end 
of a command sent over a TCP connection? What 
challenges arise if this is not handled correctly?



Since TCP is a stream-based protocol, it does not preserve message boundaries. This means the protocol must explicitly define where messages begin and end.
Solution:
(i)Use End Markers:The client appends a null byte (\0) at the end of each command before sending it to the server.
(ii)The server sends an EOF marker (0x04) at the end of the command output to signal that the transmission is complete.

Use Fixed-Length Headers (Alternative Method):
The sender includes the message length in a fixed-length header before the data.

Challenges if not handled correctly:


(i)Message Fragmentation:Multiple commands might be received together as one, leading to incorrect parsing.

(ii)Message Concatenation:
Multiple commands might be received together as one, leading to incorrect parsing.

(iii)Deadlocks & Incorrect Processing:
If the client waits indefinitely for data that was never sent, it could freeze.




3. Describe the general differences between 
stateful and stateless protocols.
Stateful Protocols:
 These keep track of past interactions. Each new 
 request can rely on previous information. An 
 example is a TCP connection or an HTTP session 
 that uses cookies.
 Pros: Can offer more personalized and efficient 
 interactions.
 Cons: More complex to manage, especially with 
 many concurrent users.


Stateless Protocols:
 Every request is independent; the server does 
 not remember past requests. Classic HTTP is 
 stateless.
 
 Pros: Easier to scale and manage because there’s 
 no session data to keep track of.
 Cons: Each request must include all necessary 
 information, which can sometimes lead to extra 
 overhead.


	The general difference between stateful and 
    stateless protocols is that stateful 
    protocols keep track of past requests and 
    each request can rely on past information but 
    
    with stateless protocols, every request is 
    independent. 

4. Our lecture this week stated that UDP is 
"unreliable". If that is the case, why would we 
ever use it?
UDP is unreliable because it does not guarantee message delivery, ordering, or retransmission. However, we still use UDP because:

(i)Speed & Low Latency:
UDP is much faster than TCP because it does not require connection setup.
This is essential for real-time applications like gaming, VoIP, and video streaming.

(ii)Supports Broadcast & Multicast:
Unlike TCP (which is always one-to-one), UDP allows one-to-many communication.
Useful for DNS lookups, live streaming, and discovery protocols.

(iii)Applications Can Handle Reliability Themselves:
Some applications, like online multiplayer games, prefer speed over reliability and retransmit only important data if needed.


5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

The Sockets API is the main interface provided by the operating system for network communication. It allows applications to:
Create sockets (socket())
Bind to an address (bind())
Send & receive data (send(), recv())
Establish TCP connections (connect(), accept())
Close the connection (close())