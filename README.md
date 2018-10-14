# SecMes [WIP]
In an effort to both brush up on C++, and learn more about sockets and
encryption libraries, I decided to write a fairly simple p2p messaging app. This
repo is the proving ground for that effort.

Now, before I go any further, I want to note that this is largely an academic
exercise. This is not intended to showcase "the best way" to do things. There
are many libraries, and languages for that matter, that would make this process
much easier. Reading through this code, you will see areas where I decided to
roll my own *X*, why?... For the same reason that I enjoy making my own bows
and knives, doing something from scratch often gives a deeper understanding and
appreciation of said thing.

This repo can be thought of as having two parts, the frontend, which is the
user facing chat app, and the backend, which is composed of the varying
implementations of socket and encryption handlers.

## Frontend
As stated above, the frontend of this project is the chat app itself. The
frontend app can be thought of as the framework in which we are testing our
backend implementations of the socket and encryption code. 

### State Machine
At the top level, the chat app is a state machine of type `ChatMachine`. At
construction, the machine is handed pointers to a `NetEngine`, a `SecEngine`
(optional), and a `ChatWindow` (more on these below). The chat app traverses
these states:

```
   +--------------------+
   |                    |
   | ConfigureChatState |
   |                    |
   +--------+-----------+
            |
            |
    +-------v----------+
    |                  |
+-->+ WaitingChatState +<--+
|   |                  |   |
|   +-------+----------+   |
|           |              |
|           |              |
|    +------v--------+     |
|    |               |     |
|    | AuthChatState +-----+
|    |               |
|    +------+--------+
|           |
|           |
|  +--------v----------+
|  |                   |
+--+ ChattingChatState |
   |                   |
   +-------------------+

```
### States
Each of the four states inherits from the `AbstractChatState`, giving them
access to some of the components owned by the chat machine. The states have the
following roles and responsibilities:
* `ConfigureChatState` - during this stage, the app loads configuration
information such as, the port to host on, the local user name, etc.
* `WaitingChatState` - during this stage, the app is waiting for either, an
incoming connection, or a request to send an outgoing connection. Once there is
a connection in place, this stage will hand things off to the `AuthChatState`.
* `AuthChatState` - this stage will, once the `SecEngine` is fleshed out, be
responsible for exchanging certs, verifying that we trust the remote end,
and allowing the user to accept/reject connections from unknown sources.
* `ChattingChatState` - yep, its a terrible name, anyhow, this is where the two
ends can actually communicate!

## Backend
### Abstractions
Given that I want to be able to try out varying types of encryption and socket
libraries, I wanted to be sure that I provided a nice abstraction between the
chat app and those libraries. For the sake of future proofing, I also wrapped
a layer of abstraction around the user interface.

Here are the three abstractions and a rough description of their interfaces:
* `NetEngine` - Any network handler should be wrapped in code that inherits from
this class. In doing so, it will define routines for doing things such as,
handling connection events, and sending and receiving messages.
* `SecEngine` - Any encryption library should be wrapped in code that inherits
from this class. What that interface should look like is still TBD.
* `ChatWindow` - The user interface, however implemented, should be wrapped in
code that inherits from this class. In doing so, it will expose a generic
interface for displaying info to the user, and getting input from the user.

### Implementations
At present, I have written the following implementations for each of the given
abstractions above:
* `NetEngine`
    * `MockNetEngine` - This guys is just an empty implementation that is quite
helpful for testing.
    * `UnixSocketNetEngine` - This class implements all of the needed interfaces
of the `NetEngine` using unix sockets.
    * Future Work - Maybe write a `boost` implementation.
* `SecEngine`
    * `TBD` - I have yet to write an implementation for the this interface. I
will probably settle on something like `libsodium` when I do.
* `ChatWindow`
    * `NcursesChatwindow` - This class implements all of the needed interfaces
of the `ChatWindow` using `ncurses`.
    * Future Work - Maybe see if I can tie in a Qt window.

That's all I have for now, more to come!
