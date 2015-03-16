# Introduction #

Find duplicates is a tool to search and erase duplicate files in windows.

This program was originally hosted here: http://www.geocities.com/hirak_99/goodies/finddups.html

I used VC++ and MFC to build it, unfortunately now I do not have access to registered version VC++ any more. The free version does not come with MFC support, so I'll probably not be editing the code in the near future. Hence I'm setting it open for enthusiasts who might want to add new features.

# Shortcuts & Hidden Features #

  1. Auto Mark and Mark Folders work depending on current selection. They will try to leave exactly one of the currently unmarked files as unmarked. They will also not attempt unmark currently marked files. To use them without regard to current selection, click on Unmark All first and then use them.
  1. Using shift key with certain buttons make them function differently. Shift+Add causes a blank folder to be inserted in the folder list at once. Shift+Remove causes all folders to be removed at once. Shift+Mark Folder automatically marks all the files which are in any of the currently selected folders.
  1. There is a menu option to Integrate to Shell. If you have this checked, you can right click any folder any time and start Find Duplicates from the popup menu in Explorer.
  1. If you need to see whether two different sets of folders have any files in common, this is precisely what Selected vs. Unselected button does. Add all the folders, then select a list of folders to define one of the sets (holding Ctrl while clicking will allow you to select multiple folders) - press the button - bingo!

# Wishlist #
Here are the todo items for the future:
  * Add an option to create symbolic links instead of deleting the files altogether. Symbolic links are supported in Windows XP and Vista.

In case you want to modify the codes, please let the project managers know. Please send a text detailing your modification along with the patch, your patch will be uploaded if approved.