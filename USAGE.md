
How to Use
==========


Add and modify minerals
-----------------------

Click on "Add new mineral" and fill the form to add a new mineral. Here a brief
description of each field:

 - Species: the main minerals in the sample, like "Fluorite", "Smoky Quartz",
   or "Quartz on Muscovite".
 - Number: and increasing number for each sample; it is given automatically if
   left blank.
 - UID: an unique identifier; this is also created automatically if left blank
   according to the rules in Settings (see below).
 - Class: the Nickel-Strunz classification for the main mineral in the sample;
   e.g. 4.DA.05 for quartz, or 3.AB.25 for fluorite. You can get it from
   mindat, if left blank it is automatically given the class X.XX.XX.
 - Chemical formula: the chemical composition of the main minerals. (optional)
 - Locality: the locality of the sample, in which mine or state it was found.
   Usually something like: "cave, state, country", e.g. "Jardinera #1 mine,
   Inca de Oro, Chile". (optional, but important!)
 - Acquisition: how and when you get possession of the sample, like "Bought at
   mineral show at ---", or "Found myself during field trip at ---". (optional)
 - Fluorescence: If the mineral is fluorescent, add fluorescence color and
   wavelenght (short-wave, long-wave). (optional)
 - Radioactivity: Indicate if the mineral if radioactive, and its activity.
   (optional)
 - Size: the dimension of the sample in whatever units you prefer (cm, inch,
   millimiters...); e.g. "1 x 2 x 1.5 cm". (optional)
 - Weight: the weight of the sample whatever units you prefer; e.g. "10 g".
   (optional)
 - Price: how much you paid for the sample, or how much it is worth. (optional)
 - Comments: any other comments you have on the sample, like special notes,
   observations, stories, ... (optional)

Once the form is filled, click "Add Mineral!" to store add data. It will then
appear on the left panel of the app: if you click on it all stored informations
will be displayed on the right panel. If you find any error, or want to add
some other information, select the mineral you want to modify in the left
panel, and click "Modify selected". A similar form to the previous one will
appear, where you can modify what you need to.


Save (and retrive) the mineral database
---------------------------------------

Once you have finished to insert all your minerals you need to save the
database you have created.  To save the database, just click to "Save database"
and pick where to save it and how to call it. The file extension is not
important, I use `.json`.  To retrive a saved database, click to "Load
database" and find the database file you want to load.

**WARNING!** By default no automatic saving is done, so if you close the
application without saving everything is lost!  


Mineral photos and images
-------------------------

MineralApp can show photos and figures about the minerals you inserted. To have
this, create a new directory where you saved the database, and store inside the
photos and images. To "attach" a photo to a minreal the filename of the photo
must start with the UID of that mineral. For example, if in your database you
have a mineral whose UID is `7.GA.05_Powellite_75`, you can save the images
`7.GA.05_Powellite_75.jpg` and `7.GA.05_Powellite_75_UVSW.jpg` inside the
`images` directory, and they will show up in the right panel of the application
when you click on that mineral. The file directory structure should be:

    parent-directory/
        my-database.json
        images/
            7.GA.05_Powellite_75.jpg
            7.GA.05_Powellite_75_UVSW.jpg
            any-other-image

Settings
--------

There are three settings:

 - Sort by: define how to sort the minerals in the left panel: "Class" will
   order them by class, "Number" will order them by number (which should be
   chronological order of addition)
 - Style: the style of the application. You can pick among some styles in the
   drop-down menu. Just cosmetics...
 - Format UID: the format of the UID for each mineral. By default it is
   `%C_%S_%N` which translate to `class_species_number`. For example for a
   Powellite (class 7.GA.05) that was added as number 75, the default class is
   `7.GA.05_Powellite_75`. You can modify the UID format as you like, remembering
   that `%C` will be substituded by the class of the mineral, `%S` by the species,
   and `%N` by the number. The format I use is `%C_%S_SC%N` to mark that that
   mineral is in the `SC` collection.

Click save to apply changes in the UID. **NOTE** If you change the UID format
you will need to manually change the filename of any image or photo to get them
recognized again.



