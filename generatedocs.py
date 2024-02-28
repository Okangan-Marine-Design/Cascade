import os

# we want to do a tree walk of the directory and find all the files with the md extension
# we want to then create a list of all the files and their paths

def get_files(directory):
    files = []
    # if the directory contains a .docignore file, we want to ignore it otherwise we want to find .md files and add them to the list
    for root, dirs, filenames in os.walk(directory):
        if '.docignore' in filenames:
            # we want to ignore the directory
            dirs[:] = []
        else:
            for filename in filenames:
                if filename.endswith('.md'):
                    files.append(os.path.join(root, filename))

    return files

def delete_old_docs(destination):
    # this function will delete markdown files from the docs directory
    # we will ignore markdown files that start with an underscore
    for root, dirs, filenames in os.walk(destination):
        for filename in filenames:
            if filename.endswith('.md') and not filename.startswith('_'):
                os.remove(os.path.join(root, filename))

    # if a folder is empty we want to delete it as well starting from the bottom of the tree
    for root, dirs, filenames in os.walk(destination, topdown=False):
        for dir in dirs:
            if not os.listdir(os.path.join(root, dir)):
                os.rmdir(os.path.join(root, dir))

def copy_docs(files, destination):
    # we are provided a list of files and there relative paths from the root directory
    # we want to move the files to the destination directory
    # we want to create the same directory structure in the destination directory

    for file in files:
        # we want to get the relative path of the file
        relative_path = os.path.relpath(file, '.')
        # we want to create the destination path
        destination_path = os.path.join(destination, relative_path)
        # we want to create the directory structure in the destination directory
        os.makedirs(os.path.dirname(destination_path), exist_ok=True)
        # we want to copy the file to the destination directory
        os.system(f'cp {file} {destination_path}')

# generate the sidebar based on the files array and write it to the _sidebar.md file
def generate_sidebar(files, destination='./docs'):
    sidebar = '- [Home](/)\n'
    
    for file in files:
        # we want to get the relative path of the file
        relative_path = os.path.relpath(file, './')
        # we want to open the file find a line that starts with # and add it to the sidebar
        with open(file, 'r') as f:
            for line in f:
                if line.startswith('#'):
                    sidebar += f'- [{line[1:].strip()}]({relative_path})\n'
                    break
                

    print(sidebar)
    # Write the sidebar to the _sidebar.md file
    with open(os.path.join(destination, '_sidebar.md'), 'w') as f:
        f.write(sidebar)

def main():
    # we want to get a list of all the files in the directory
    files = get_files('.')

    # get length of list
    number_of_files = len(files)
    print(f'We have {number_of_files} documents to generate.')
    # we want to delete the old docs
    delete_old_docs('./docs')
    # we want to copy the files to the docs directory
    copy_docs(files, './docs')
    # we want to generate the sidebar
    generate_sidebar(files)

if __name__ == '__main__':
    main()