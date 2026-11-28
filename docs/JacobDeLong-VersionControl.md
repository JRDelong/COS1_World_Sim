# Instructions

Update this document where indicated [look for the brackets!]. Replace text inside the brackets with your own information. For example: Course Name should be the name of this course, and not the generic words "Course Name".

<br>

## [ COS119-0 02]

- **[ Jacob DeLong ]**
- **[ Nov 30th 2026 ]**

This paper addresses some of the topic matter covered in research and activity this week. Be sure to include reference links below to the research and information you used to complete this assignment.

## Topic: Terminal

Professional developers use Terminal daily. It's essential to understand some fundamental commands to use the application.

Update the information below to demonstrate your knowledge on this topic.

**1. Using Terminal, there are essential commands to know.**

List the correct Terminal commands to do the actions listed below. Replace **CMD** with the correct command sequence. You can keep or enhance the brief description.

**The last bullet provides an example**.

- [ clear ]: Clear the Screen
- [ pwd]: Print the "Working Directory"
- [ ls]: List files and folders
- [ ls -a]: List files and folders, including invisible files
- [ ls -lah ]: List all files and folders, in human readable form
- [ cd]: Change directory
- [ cd / ]: Change directory, go to root directory
- [ cd~]: Change directory and go to user home directory
- [ cd .. ]: Change directory, go up one folder level
- [ cd .. / .. ]: Change directory, go up two folder levels
- [ cd~/Desktop ]: Change directory to my desktop!

**2. Using Terminal...**

**Folder Drop:** Try typing "cd" followed by a space, and then drag a folder into terminal and press return. Test this out and describe your results below.

[ Terminal automatically types out full path to the dropped folder and switches working directory to it as soon as enter is pressed. ]

## Topic: Version Control & Git

Version control, also known as revision control, records changes to a file or set of files over time so that you can recall specific versions later. In this class, we are learning Git. Update the information below where indicated.

**1. There are three types of version control.**

[ Local, Centralized, and Distributed. Local keeps simple database on the local computer and tracks all changes made to files over time. Centralized uses a single central server that stores all version files. Many people can connect and work same files. Distributed clones the entire repository like Visual Studio does including full commit history to local computer. ]

**2. Using Terminal, there are also essential Git commands to know.**

List the correct Git commands to do the actions listed below in Terminal. Replace CMD with the correct command and keep or enhance the brief description.

- [ git clone [url] ]: Clone a repository
- [ git config -- global user.name "[Your Name]" ]: Set-up a global user name
- [ git config -- global user.email "[your_email@example.com]" ]: Set-up a global email address (to match my GitHub account email)
- [ git status ]: Shows the current state of your directory and staging area
- [ git add [file_name] ]: Add modified files to the next commit
- [ git commit -m "[Your commit message]" ]: Make a commit with a new message
- [ git log ]: Show my commit history
- [ git help ]: Show Git's help screen

**3. Connecting to GitHub using Terminal.**
HTTPS is the the correct way to connect to GitHub in this course. Describe how you connect to GitHub from Terminal using this protocol. What steps do you take?

[ 1. Generate a Personal Access Token (PAT) on GitHub
1. Log in to **GitHub** in your browser.
2. Click your profile picture (top-right) and go to **Settings** > **Developer settings** (bottom of the left sidebar).
3. Select **Personal access tokens** > **Tokens (classic)** (or **Fine-grained tokens**).
4. Click **Generate new token**, give it a name (e.g., `Terminal Access`), set an expiration date, and check the **`repo`** scope box.
5. Click **Generate token** and **copy the token immediately** (you will not be able to see it again).

---

[2. Copy the Repository's HTTPS URL]
1. Go to the main page of your repository on GitHub.
2. Click the green **`<> Code`** button.
3. Ensure the **HTTPS** tab is selected and copy the URL (it looks like `https://github.com`).

---

[3. Connect in Terminal]
1. Navigate to your desired folder in Terminal (`cd ~/Desktop`).
2. run git clone https://github.com
3. run git remove -v

[4. Authenticate When Prompted]
1. Terminal will prompt you for your **Username**: enter your GitHub username.
2. Terminal will prompt you for your **Password**: paste your **Personal Access Token** (not your GitHub website password) and press Return. _(Note: For security, Terminal will not display characters or dots as you paste/type the token.)_
3. Once entered, your system's credential manager (such as macOS Keychain or Git Credential Manager) caches the token so you don't have to re-enter it on future commands.

**4. Using .gitignore and Why it's Important**  
Most repositories contain a .gitignore file.

- What is the purpose of this file?
  <br>
  [A `.gitignore` file tells Git which files or folders to intentionally ignore and leave untracked. It keeps your repository clean by preventing temporary, system-generated, compiled, or sensitive files from being uploaded to GitHub.]

- What is the "**.DS_Store**" file and why would you want to ignore it?
  <br>
  [- `.DS_Store` (Desktop Services Store) is a hidden metadata file automatically created by macOS Finder to store custom folder attributes (such as icon positions and window view settings). You want to ignore it because it is specific to your personal Mac's display preferences, serves no purpose in the project's code, and creates unnecessary clutter and merge conflicts for collaborators.]

- What other file or folder would you want to add to a .gitignore file and why?
  <br>
  [- - **Build and compiled output folders** (such as `bin/`, `obj/`, `Debug/`, or `Release/` in C++/C# projects) because they contain machine-specific compiled binaries that can be regenerated anytime from the source code.
    - **IDE/editor configuration folders** (such as `.vs/` for Visual Studio or `.vscode/`) because they store local user preferences, window layouts, and temporary caches.
    - **Environment or secret files** (such as `.env` or `config.json`) because they often contain sensitive API keys, passwords, or database credentials that should never be exposed publicly on GitHub.]

<br>

# Reference Links

Replace the example references below with your own links and recommended resources. It is acceptable to provide multiple links for a single topic and to use material provided to you in this class. You are encouraged to link to your own independent research as well.

[ Research Summary: What resource(s) did you find most helpful this past week and why? ]

**Terminal Commands**  
[Site Address](https://www.google.com)

**Three Types of Version Control**  
[Site Address](https://www.google.com/)

**Git Commands**  
[Site Address](https://www.google.com)

**Connecting to GitHub using Terminal**  
[Site Address](https://www.google.com)

**Using .gitignore and Why it's Important**  
[Site Address](https://google.com/search)
