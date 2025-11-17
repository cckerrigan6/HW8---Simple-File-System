 /*******************************************************
 * CS 2130 Homework 8
 * Fall 2025 - Simplified File System 
 *
 * Computing ID: qff9sw
 * Sources: Cite your sources, including any GenAI
 *          usage.  (See syllabus for full details)
 *******************************************************/

#include "filesystem.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

dir_node *create_dir(const char *name) {//creates a new directory struct in memory with the given name and returns a pointer to that directory	
	//if the name is NULL, return NULL
        if(name == NULL) return NULL;

	//assign memory on the heap for a new directory
	dir_node *newdir = malloc(sizeof(dir_node));

	//check if malloc failed
	if(newdir == NULL) return NULL;

	//now pointer points to the copy of the name in the heap
	newdir->name = strdup(name);

        //check if strdup failed
	if(newdir->name == NULL){
		free(newdir);
		return NULL;
	}

	//initialize all  pointers in the struct to NULL
	newdir->parent = NULL;
	newdir->next = NULL;
	newdir->subdirs = NULL;
	newdir->files = NULL;

	//return pointer to new directory
	return newdir;
}

dir_node *root_dir(void *node) {//returns a pointer to the highest-level directory, given a pointer to either a directory node or a file node
	//check if input node is NULL
	if(node == NULL) return NULL;

	//Create a pointer to traverse up to the root directory
	dir_node *current_dir;

	//create a file_node pointer
	file_node *f = (file_node *)node;

	//If the node is a file, start from parent directory
	if(f->parent){
		current_dir = f->parent;
	}else{//if the node is a directory, start from the directory
		current_dir = (dir_node *)node;
	}

	//traverse up until reaching the root
	while(current_dir->parent){
		current_dir = current_dir ->parent;

	}//return the root directory 
	return current_dir;
}

file_node *first_file(dir_node *dir) {//returns a pointer to the first file in the directory, given a pointer to any directory node
	//check if pointer is NULL
	if(dir == NULL) return NULL;

	//return the first file in the directory (files pointer points to the first file)
	return dir->files;
}

file_node *last_file(dir_node *dir) {//returns a pointer to the last node file in the directory, given a pointer to any directory node
	//check if dir is NULL
	if(dir == NULL) return NULL;

	//check if the directory has no files
	if(dir->files == NULL) return NULL;

	//start at the first file in the directory
	file_node *current_file = dir->files;

	//traverse the linked list until the last file
	while(current_file->next != NULL){
		current_file = current_file->next;
	}

	//return a pointer to the last file 
	return current_file;
}

unsigned long dir_entries(dir_node *dir) {//Returns the number of entries in the directory, given a pointer to any directory node. Number of entries = sum of number of files and subdirectories
	//check if dir is NULL
	if(dir == NULL) return 0;

	//Initialize a variable for the number of entries(files + subdirectories)
	unsigned long num_entries = 0;

	//start at the first file
	file_node *file = dir->files;
	//Traverse the linked list of files, adding each file to the num_entries counter
	while(file != NULL){
		num_entries++;
		file = file->next;
	}

	//Start at the first subdirectory
	dir_node *subdir = dir->subdirs;
	//Traverse the linked list of subdirectories, adding each subdirectory to the num_entries counter
	while(subdir != NULL){
		num_entries++;
		subdir = subdir->next;
	}

	//return the total number of entries
	return num_entries;
}

file_node *find_file(dir_node *dir, const char *name) {//returns the pointer to a file node in the directory with the given name
	//check if the dir or name is NULL
	if(dir == NULL || name == NULL) return NULL;

	//Start at the first file
	file_node *file = dir->files;
	//Traverse the linked list of files, comparing the current file's name to the input name
	while(file != NULL){
		if(strcmp(file->name, name) == 0){ //strcmp returns 0 if the two names are identical
			return file;
		}

		//Moves the pointer to the next file in the linked list
		file = file->next;
	}

	//if I reach the end of the linked list without finding a match then return NULL
	return NULL;
}


dir_node *find_dir(dir_node *dir, const char *name) {//returns the pointer to a subdirectory node in the directory with the given name
	//check if the dir or name is NULL
	if(dir == NULL || name == NULL) return NULL;

	//Start at the first subdirectory
	dir_node *subdir = dir->subdirs;
	//Traverse the linked list of subdirectories, comparing the current subdirectory's name to the input name
	while(subdir != NULL){
		if(strcmp(subdir->name, name) == 0){//strcmp returns 0 if the two names are identical
			return subdir;
		}

		//Moves the pointer to the next subdirectory in the linked list
		subdir = subdir->next;
	}

	//if I reach the end of the linked list without finding a match then return NULL
	return NULL;
}

file_node *remove_file(dir_node *dir, file_node *file) {//Given a pointer to a directory node and a file node, remove that file from the list of files in that directory
	//Check to see if dir or file is NULL
	if(dir == NULL || file == NULL) return NULL;

	//Start at the first file
	file_node *current_node = dir->files;
	//initialize and keep track of the node before the current node
	file_node *previous_node = NULL;

	//traverse the linked list of files
	while(current_node != NULL){
		if(current_node == file){//check to see if the current node is the file to remove
			file_node *node_after_removed = current_node->next;//create a pointer to the node after the one that will be removed
			if(previous_node == NULL){//removing the first file of a list
				dir->files = node_after_removed;
			}else{//removing a node in the middle or at the end
				previous_node->next = node_after_removed;
			}

			//free the memory assigned for the file node
			free(current_node);

			//return a pointer to the node that occupies the removed node's position
			return node_after_removed;
		}

		//move to the next position in the list
		previous_node = current_node;
		current_node = current_node->next;
	}
	//after going through the list if the file wasn't found return NULL(nothing to delete)
	return NULL;
}

dir_node *remove_dir(dir_node *dir, dir_node *subdir) {//Given a pointer to a directory node and a subdirectory node, remove that subdirectory from the list of subdirectories in that directory
	//check if dir or subdir is NULL
	if(dir == NULL || subdir == NULL) return NULL;

	//start at the beginning of the parent's subdirs list
	dir_node *current_dir = dir->subdirs;
	//initialize and keep track of the node before the current node
	dir_node *previous_dir = NULL;

	//Traverse the subdirectory list
	while(current_dir != NULL){
		if(current_dir == subdir){//check to see if the current directory is the one to be removed
			dir_node *node_after_removed = current_dir->next;//create a pointer to the node after the one to be removed
			if(previous_dir == NULL){//removing the first subdir in the list
				dir->subdirs = node_after_removed;
			}else{//removing a subdir in the middle or at the end (not first in list)
				previous_dir->next = node_after_removed;
			}

			//The subdirectories of the subdirectory removed are added at the end of the directory node's list of subdirectories
			if(current_dir->subdirs != NULL){
				dir_node *subdir_end = dir->subdirs;//start at beginning of parent's subdirectory list
				if(subdir_end == NULL){//if parent has no subdirectories
					dir->subdirs = current_dir->subdirs;
				}else{//traverse until the end, then attach
					while(subdir_end->next != NULL){
						subdir_end = subdir_end->next;
					}
					subdir_end->next = current_dir->subdirs;
				}

				//Update the parent pointer for all the subdirectories updated
				dir_node *update_parent_pointer = current_dir->subdirs;
				while(update_parent_pointer != NULL){
					update_parent_pointer->parent = dir;
					update_parent_pointer = update_parent_pointer->next;
				}
			}

			//The subdirectory files of the subdirectory removed are added as files at the end of the directory node's list of files
			if(current_dir->files != NULL){
				file_node *file_end = dir->files;//Start at beginning of parent's file list
				if(file_end == NULL){//If parent has no files
					dir->files = current_dir->files;
				}else{//traverse until the end, then attach
					while(file_end->next != NULL){
						file_end = file_end->next;
					}
					file_end->next = current_dir->files;
				}

				//update the parent pointer for all the files updated
				file_node *update_pp_file = current_dir->files;
				while(update_pp_file != NULL){
					update_pp_file->parent = dir;
					update_pp_file = update_pp_file->next;
				}
			}

			free(current_dir);//free memory of the removed subdir

			return node_after_removed;//returns a pointer to the node that now occupies the same position in the list that the removed node occupied
		}

		// move to next position in the list
		previous_dir = current_dir;
		current_dir = current_dir->next;
	}

	//after going through the list if the directory wasn't found return NULL(nothing to delete)
	return NULL;
}

file_node *insert_file(dir_node *dir, const char *name, int size, const char *after) {//extend a directory's list of files by one by adding a new file node with the given name and file size
	//check if dir or name is NULL
	if(dir == NULL || name == NULL) return NULL;

	//assign memory on the heap for a new file node
	file_node *new_file = malloc(sizeof(file_node));
	//check to see if malloc fails
	if(new_file == NULL) return NULL;

	//Duplicate name to be stored in new file node
	new_file->name = strdup(name);
	//check to see if strdup failed
	if(new_file->name == NULL){
		free(new_file);
		return NULL;
	}

	//initialize size, parent, and next(part of file_node struct)
	new_file->size = size;
	new_file->parent = dir;
	new_file->next = NULL;

	//if after is NULL, insert file to the head of the file list
	if(after == NULL){
		new_file->next = dir->files;
		dir->files = new_file;
		return new_file;
	}

	//search for a file called "after" in the file list,if found insert the new file immediately following the node with name matching after
	file_node *current_file = dir->files;
	while(current_file != NULL){
		if(strcmp(current_file->name, after) == 0){//if file name and after match, strcmp is 0
			new_file->next = current_file->next;
			current_file->next = new_file;
			return new_file;
		}
		current_file = current_file->next;
	}

	//if no file matching the name after is found, insert the new file at the end of the file list
	current_file = dir->files;
	if(current_file == NULL){//checks to see if the file list was empty
		dir->files = new_file;
	}else{//traverse to the last file in the list
		while(current_file->next != NULL){
			current_file = current_file->next;
		}
		current_file->next = new_file;
	}

	//return pointer to the new file inserted
	return new_file;
}

dir_node *insert_dir(dir_node *dir, const char *name, const char *after) {//extend a directory's list of subdirectories by one by adding a new directory node with the given name
	//check to see if dir or name is NULL
	if(dir == NULL || name == NULL) return NULL;

	//assign memory on the heap for a new directory
	dir_node *new_dir = malloc(sizeof(dir_node));
	//check to see if malloc failed
	if(new_dir == NULL) return NULL;

	//duplicate name to be stored in new directory node
	new_dir->name = strdup(name);
	//check to see if strdup failed
	if(new_dir->name == NULL){
		free(new_dir);
		return NULL;
	}

	//initialize parent, next,subdirs, files(part of dir_node struct)
	new_dir->parent = dir;
	new_dir->next = NULL;
	new_dir->subdirs = NULL;
	new_dir->files = NULL;

	//if after is NULL, insert directory to the head of teh subdirectory list
	if(after == NULL){
		new_dir->next = dir->subdirs;
		dir->subdirs = new_dir;
		return new_dir;
	}

	//search for a directory with an identical name to after,if found insert the new directory immediately following the node with name matching after
	dir_node *current_dir = dir->subdirs;
	while(current_dir != NULL){
		if(strcmp(current_dir->name, after) == 0){//if directory name and after match, strcmp will return 0
			new_dir->next = current_dir->next;
			current_dir->next = new_dir;
			return new_dir;
		}
		current_dir = current_dir->next;
	}

	//if no directory matching the name after is found, insert directory at the end of the list of subdirectories
	current_dir = dir->subdirs;
	if(current_dir == NULL){//check to see if the subdirectory is empty
		dir->subdirs = new_dir;
	}else{//traverse to the last directory in the list
		while(current_dir->next != NULL){
			current_dir = current_dir->next;
		}
		current_dir->next = new_dir;
	}
	//return pointer to new directory inserted
	return new_dir;
}



// The following function has been provided for you
void directory_list(dir_node *root, int indent) {
  // Stop if the root is null (nowhere to go)
  if (root == NULL)
    return;

  // If indent is 0, we need to start at 4 for correct indentation
  if (!indent)
    indent = 4;
  
  // Print the current directory node
  printf("%*s %s (%ld entries)\n", indent, "-(D)", root->name, dir_entries(root));

  // Recursively print the subdirectories and their contents
  indent += 4;
  for (dir_node *i = root->subdirs; i != NULL; i = i->next) {
    directory_list(i, indent);
  }

  // Print out the files (no recursion needed)
  for (file_node *i = root->files; i != NULL; i = i->next) {
    printf("%*s %s (%d bytes)\n", indent, "-(F)", i->name, i->size);
  }
}

// Tester for remove_dir
void test_remove_dir() {
    dir_node *first = remove_dir(NULL, NULL);
    dir_node *f0 = create_dir("example");

    dir_node *d = create_dir("Dir A");
    dir_node *second = remove_dir(d, NULL);
    dir_node *third = remove_dir(d, f0);
    
    dir_node *e = create_dir("Dir B");
    dir_node *f1 = insert_dir(e, "f1", NULL);
    dir_node *fourth = remove_dir(e, f0);
    dir_node *fifth = remove_dir(e, f1);

    puts("remove_dir zero or one:");
    
    if (first != NULL) puts("- wrong value on NULL directory");
    else if (second != NULL) puts("- wrong value on NULL dir");
    else if (third != NULL) puts("- wrong value on dir not in list");
    else if (fourth != NULL) puts("- wrong value on dir not in list 2");
    else if (fifth != NULL) puts("- wrong value when removing only dir");
    else if (e->subdirs != NULL) puts("- did not update directory");
    else puts("+ passed all tests");
}


// Test your code in main
int main(int argc, const char *argv[]) {
  dir_node *d = create_dir("Top");
  dir_node *fd = insert_dir(d, "First Dir", NULL);
  dir_node *sd = insert_dir(d, "Second Dir", "First Dir"); 

  //tester code for create_dir
  dir_node *test = create_dir("Test_create_dir");
  printf("Name: %s, parent: %s\n", test->name, test->parent == NULL ? "NULL" : "Not Null");

  //tester code for root_dir
  dir_node *first = create_dir("First"), *second = create_dir("Second"); second->parent = first;
  file_node *f = malloc(sizeof(file_node)); f->name = strdup("file"); f->parent = second; f->size =1;
  printf("Root of second: %s\n", root_dir(second)->name);
  printf("Root of file: %s\n", root_dir(f)->name);

  //insert files
  insert_file(d, "First File", 42, NULL);
  insert_file(fd, "Second File", 67, NULL);
  insert_file(fd, "Third File", 21, "Second File");
  insert_file(sd, "Fourth File", 0, NULL);

  //tester for insert_file
  printf("Top:\n");
  file_node *finsert = d->files;
  while(finsert != NULL){
  	printf("  %s (%d bytes)\n", finsert->name, finsert->size);
	finsert = finsert->next;
  }
  printf("Files in First Directory:\n");
  finsert = fd->files;
  while(finsert != NULL){
  	printf("  %s (%d bytes)\n", finsert->name, finsert->size);
	finsert = finsert->next;
  }
  printf("Files in Second Directory:\n");
  finsert = sd->files;
  while(finsert != NULL){
        printf("  %s (%d bytes)\n", finsert->name, finsert->size);
        finsert = finsert->next;
  }

  //tester for first_file
  file_node *first_fd = first_file(fd);
  if(first_fd != NULL) printf("First file in first directory: %s\n", first_fd->name);

  //tester for last_file
  file_node *last_fd = last_file(fd);
  if(last_fd != NULL) printf("Last file in First Directory: %s\n", last_fd->name);

  //tester for dir_entries
  dir_node *d1 = create_dir("dir_entries_test");
  insert_dir(d1, "subdir1", NULL);
  insert_file(d1, "file1", 10, NULL);
  printf("Number of Entries = %lu (expected 2)\n", dir_entries(d1));

  //tester for find_file
  file_node *f1 = find_file(d, "First File");
  if(f1 != NULL) printf("Found file: %s\n", f1->name);
  file_node *f2 = find_file(fd, "HEY");
  if(f2 == NULL) printf("File not found\n");

  //tester for find_dir
  dir_node *find1 = find_dir(d, "First Dir");
  if(find1 != NULL) printf("Found subdirectory: %s\n", find1->name);
  dir_node *find2 = find_dir(d,"Please work");
  if(find2 == NULL) printf("Subdirectory not found\n");

  //tester for remove_file
  dir_node *removef = create_dir("Remove_file_test");
  file_node *r1 = insert_file(d, "File1", 10, NULL);
  file_node *r2 = insert_file(d, "File2", 20, "File1");
  file_node *r3 = insert_file(d, "File3", 30, NULL);
  printf("Beginning Files: ");
  file_node *current = d->files;
  while(current){
  	printf("%s ", current->name);
	current = current->next;
  }printf("\n");
  printf("Remove File3");
  remove_file(d, r3);
  printf("after removing File3: ");
  current = d->files;
  while(current){
  	printf("%s ", current->name);
  	current = current->next;
  }printf("\n");
  printf("Remove File2");
  remove_file(d, r2);
  current = d->files;
  while(current){
  	printf("%s ", current->name);
	current= current->next;
  }printf("\n");

  //Tester for remove_dir
  test_remove_dir();

  // print the directory listing starting at the root for sd
  directory_list(root_dir(sd), 0);

  return 0;
}
