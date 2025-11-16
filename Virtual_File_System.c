#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 5000
#define MAX_NAME_LEN 50
#define INPUT_BUF 1024

typedef struct FreeBlock
{
  int index;
  struct FreeBlock *prev;
  struct FreeBlock *next;
} FreeBlock;

typedef struct FileNode
{
  char name[MAX_NAME_LEN + 1];
  int isDirectory;
  struct FileNode *parent;
  struct FileNode *child;
  struct FileNode *nextSibling;
  struct FileNode *prevSibling;
  int *blockPointers;
  int numBlocks;
  size_t contentSize;

} FileNode;

unsigned char **virtualDisk = NULL;

FreeBlock *freeHead = NULL;
FreeBlock *freeTail = NULL;
int freeCount = 0;

FileNode *root = NULL;
FileNode *cwd = NULL;

static char *trim(char *s);
static char *unescapeString(const char *src);

void appendFreeBlock(int index);
static void initFreeList(void);
static int allocateBlockFromHead(void);
static void freeBlockToTail(int index);

void initVFS(void);
void cleanupVFS(void);
static void freeNodeRecursive(FileNode *node);

static FileNode *createNode(const char *name, int isDirectory);
static void insertChild(FileNode *parent, FileNode *node);
static FileNode *findChild(FileNode *parent, const char *name);
static void removeChild(FileNode *parent, FileNode *node);

static void cmd_mkdir(const char *arg);
static void cmd_create(const char *arg);
static void cmd_ls(void);
static void cmd_pwd(void);
static void cmd_cd(const char *arg);

static void printPrompt(void);
static void startShell(void);

static void cmd_write(const char *filename, const char *text);
static void cmd_read(const char *filename);
static void cmd_delete(const char *filename);
static void cmd_rmdir(const char *dirname);
static void cmd_df(void);

int main()
{
  initVFS();
  startShell();
  cleanupVFS();
  return 0;
}

static char *trim(char *s)
{
  if (!s)
  {
    return s;
  }
  while (*s && isspace((unsigned char)*s))
  {
    s++;
  }
  if (!*s)
  {
    return s;
  }
  char *end = s + strlen(s) - 1;
  while (end > s && isspace((unsigned char)*end))
  {
    *end-- = '\0';
  }
  return s;
}

void appendFreeBlock(int index)
{
  FreeBlock *node = (FreeBlock *)malloc(sizeof(FreeBlock));
  if (!node)
  {
    fprintf(stderr, "appendFreeBlock: malloc failed\n");
    exit(1);
  }
  node->index = index;
  node->next = NULL;
  node->prev = freeTail;

  if (freeTail)
  {
    freeTail->next = node;
  }
  freeTail = node;

  if (!freeHead)
  {
    freeHead = node;
  }

  freeCount++;
}

static void initFreeList()
{
  freeHead = freeTail = NULL;
  freeCount = 0;
  for (int i = 0; i < NUM_BLOCKS; i++)
  {
    appendFreeBlock(i);
  }
}

static int allocateBlockFromHead()
{
  if (!freeHead)
  {
    return -1;
  }
  FreeBlock *f = freeHead;
  int idx = f->index;
  freeHead = f->next;
  if (freeHead)
  {
    freeHead->prev = NULL;
  }
  else
  {
    freeTail = NULL;
  }
  free(f);
  freeCount--;
  return idx;
}

static void freeBlockToTail(int index)
{
  appendFreeBlock(index);
}

void initVFS()
{
  printf("Initializing Virtual File System....\n");

  virtualDisk = (unsigned char **)malloc(NUM_BLOCKS * sizeof(unsigned char *));
  if (!virtualDisk)
  {
    printf("Error: Unable to allocate virtual disk.\n");
    exit(1);
  }
  for (int i = 0; i < NUM_BLOCKS; i++)
  {
    virtualDisk[i] = (unsigned char *)malloc(BLOCK_SIZE * sizeof(unsigned char));
    memset(virtualDisk[i], 0, BLOCK_SIZE);
  }

  initFreeList();

  root = (FileNode *)malloc(sizeof(FileNode));
  if (!root)
  {
    fprintf(stderr, "initVFS: root allocation failed\n");
    exit(1);
  }
  strncpy(root->name, "/", MAX_NAME_LEN);
  root->name[MAX_NAME_LEN] = '\0';
  root->isDirectory = 1;
  root->parent = NULL;
  root->child = NULL;
  root->nextSibling = root->prevSibling = NULL;
  root->blockPointers = NULL;
  root->numBlocks = 0;
  root->contentSize = 0;

  cwd = root;

  printf("VFS initialized successfully.\n");
  printf("Total Blocks: %d | Free Blocks: %d\n", NUM_BLOCKS, freeCount);
}

static void freeNodeRecursive(FileNode *node)
{
  if (!node)
  {
    return;
  }
  if (node->isDirectory)
  {
    while (node->child)
    {
      FileNode *c = node->child;
      if (c->nextSibling == c)
      {
        node->child = NULL;
      }
      else
      {
        c->prevSibling->nextSibling = c->nextSibling;
        c->nextSibling->prevSibling = c->prevSibling;
        if (node->child == c)
        {
          node->child = c->nextSibling;
        }
      }
      freeNodeRecursive(c);
    }
  }
  else
  {
    if (node->blockPointers)
    {
      for (int i = 0; i < node->numBlocks; ++i)
      {
        freeBlockToTail(node->blockPointers[i]);
      }
      free(node->blockPointers);
      node->blockPointers = NULL;
      node->numBlocks = 0;
      node->contentSize = 0;
    }
  }
  free(node);
}

void cleanupVFS()
{
  if (root)
  {
    freeNodeRecursive(root);
    root = NULL;
  }
  cwd = NULL;

  FreeBlock *cur = freeHead;
  while (cur)
  {
    FreeBlock *n = cur;
    cur = cur->next;
    free(n);
  }
  freeHead = freeTail = NULL;
  freeCount = 0;

  if (virtualDisk)
  {
    for (int i = 0; i < NUM_BLOCKS; i++)
    {
      free(virtualDisk[i]);
    }
    free(virtualDisk);
    virtualDisk = NULL;
  }
  printf("Memory released. Exiting program...\n");
}

static FileNode *createNode(const char *name, int isDirectory)
{
  if (!name)
  {
    return NULL;
  }
  if (strlen(name) > MAX_NAME_LEN)
  {
    return NULL;
  }
  FileNode *n = (FileNode *)malloc(sizeof(FileNode));
  if (!n)
  {
    return NULL;
  }
  strncpy(n->name, name, MAX_NAME_LEN);
  n->name[MAX_NAME_LEN] = '\0';
  n->isDirectory = isDirectory ? 1 : 0;
  n->parent = NULL;
  n->child = NULL;
  n->nextSibling = n->prevSibling = NULL;
  n->blockPointers = NULL;
  n->numBlocks = 0;
  n->contentSize = 0;
  return n;
}

static void insertChild(FileNode *parent, FileNode *node)
{
  if (!parent || !node)
    return;
  node->parent = parent;
  if (!parent->child)
  {
    parent->child = node;
    node->nextSibling = node->prevSibling = node;
  }
  else
  {
    FileNode *head = parent->child;
    FileNode *tail = head->prevSibling;
    tail->nextSibling = node;
    node->prevSibling = tail;
    node->nextSibling = head;
    head->prevSibling = node;
  }
}

static FileNode *findChild(FileNode *parent, const char *name)
{
  if (!parent || !parent->child)
  {
    return NULL;
  }
  FileNode *start = parent->child;
  FileNode *cur = start;
  do
  {
    if (strcmp(cur->name, name) == 0)
    {
      return cur;
    }
    cur = cur->nextSibling;
  } while (cur != start);
  return NULL;
}

static void removeChild(FileNode *parent, FileNode *node)
{
  if (!parent || !node || parent->child == NULL)
  {
    return;
  }
  if (node->nextSibling == node)
  {
    parent->child = NULL;
  }
  else
  {
    node->prevSibling->nextSibling = node->nextSibling;
    node->nextSibling->prevSibling = node->prevSibling;
    if (parent->child == node)
    {
      parent->child = node->nextSibling;
    }
  }
  node->nextSibling = node->prevSibling = NULL;
  node->parent = NULL;
}

static void cmd_mkdir(const char *arg)
{
  if (!arg)
  {
    printf("Usage: mkdir <name>\n");
    return;
  }
  char name[MAX_NAME_LEN + 1];
  if (strlen(arg) > MAX_NAME_LEN)
  {
    printf("Error: name too long (max %d chars)\n", MAX_NAME_LEN);
    return;
  }
  strncpy(name, arg, MAX_NAME_LEN);
  name[MAX_NAME_LEN] = '\0';

  if (findChild(cwd, name))
  {
    printf("Name '%s' already exists in current directory.\n", name);
    return;
  }
  FileNode *d = createNode(name, 1);
  if (!d)
  {
    printf("Failed to create directory node.\n");
    return;
  }
  insertChild(cwd, d);
  printf("Directory '%s' created successfully.\n", name);
}

static void cmd_create(const char *arg)
{
  if (!arg)
  {
    printf("Usage: create <name>\n");
    return;
  }
  if (strlen(arg) > MAX_NAME_LEN)
  {
    printf("Name too long (max %d chars)\n", MAX_NAME_LEN);
    return;
  }
  if (findChild(cwd, arg))
  {
    printf("'%s' already exists in current directory.\n", arg);
    return;
  }
  FileNode *f = createNode(arg, 0);
  if (!f)
  {
    printf("Failed to create file node.\n");
    return;
  }
  insertChild(cwd, f);
  printf("File '%s' created successfully.\n", arg);
}

static void cmd_ls(void)
{
  if (!cwd->child)
  {
    printf("(empty)\n");
    return;
  }
  FileNode *start = cwd->child;
  FileNode *cur = start;
  do
  {
    if (cur->isDirectory)
    {
      printf("%s/\n", cur->name);
    }
    else
    {
      printf("%s\n", cur->name);
    }
    cur = cur->nextSibling;
  } while (cur != start);
}

static void cmd_pwd(void)
{
  if (cwd == root)
  {
    printf("/\n");
    return;
  }
  char names[128][MAX_NAME_LEN + 1];
  int top = 0;
  FileNode *cur = cwd;
  while (cur && cur != root)
  {
    strncpy(names[top], cur->name, MAX_NAME_LEN);
    names[top][MAX_NAME_LEN] = '\0';
    top++;
    cur = cur->parent;
  }
  for (int i = top - 1; i >= 0; --i)
  {
    printf("/%s", names[i]);
  }
  printf("\n");
}

static void cmd_cd(const char *arg)
{
  if (!arg)
  {
    printf("Usage: cd <dir>\n");
    return;
  }
  if (strcmp(arg, "/") == 0)
  {
    cwd = root;
    printf("Moved to /\n");
    return;
  }
  if (strcmp(arg, "..") == 0)
  {
    if (cwd->parent)
    {
      cwd = cwd->parent;
      if (cwd == root)
      {
        printf("Moved to /\n");
      }
      else
      {
        printf("Moved to ");
        cmd_pwd();
      }
    }
    else
    {
      printf("Already at root\n");
    }
    return;
  }
  FileNode *target = findChild(cwd, arg);
  if (!target)
  {
    printf("Directory not found: %s\n", arg);
    return;
  }
  if (!target->isDirectory)
  {
    printf("'%s' is not a directory\n", arg);
    return;
  }
  cwd = target;

  printf("Moved to ");
  cmd_pwd();
}

static void printPrompt(void)
{
  if (cwd == root)
  {
    printf("/ > ");
  }
  else
  {
    printf("%s > ", cwd->name);
  }
}

static void startShell(void)
{
  char input[INPUT_BUF];
  printf("Compact VFS - ready. Type 'exit' to quit.\n");
  while (1)
  {
    printPrompt();
    if (!fgets(input, sizeof(input), stdin))
    {
      printf("\nEOF received. Exiting shell.\n");
      break;
    }
    char *line = trim(input);
    if (strlen(line) == 0)
    {
      continue;
    }

    char *cmd = strtok(line, " \t\n");
    if (!cmd)
    {
      continue;
    }

    if (strcmp(cmd, "exit") == 0)
    {
      break;
    }
    else if (strcmp(cmd, "mkdir") == 0)
    {
      char *arg = strtok(NULL, " \t\n");
      cmd_mkdir(arg);
    }
    else if (strcmp(cmd, "create") == 0)
    {
      char *arg = strtok(NULL, " \t\n");
      cmd_create(arg);
    }
    else if (strcmp(cmd, "ls") == 0)
    {
      cmd_ls();
    }
    else if (strcmp(cmd, "pwd") == 0)
    {
      cmd_pwd();
    }
    else if (strcmp(cmd, "cd") == 0)
    {
      char *arg = strtok(NULL, " \t\n");
      if (!arg)
      {
        printf("Usage: cd <dir>\n");
        continue;
      }
      cmd_cd(arg);
    }
    else if (strcmp(cmd, "write") == 0)
    {
      char *fname = strtok(NULL, " \t\n");
      char *data = strtok(NULL, "\n");
      if (data && data[0] == '"')
      {
        data++;
        char *end = strrchr(data, '"');
        if (end)
        {
          *end = '\0';
        }
      }
      cmd_write(fname, data);
    }
    else if (strcmp(cmd, "read") == 0)
    {
      char *fname = strtok(NULL, " \t\n");
      cmd_read(fname);
    }
    else if (strcmp(cmd, "delete") == 0)
    {
      char *fname = strtok(NULL, " \t\n");
      cmd_delete(fname);
    }
    else if (strcmp(cmd, "rmdir") == 0)
    {
      char *dname = strtok(NULL, " \t\n");
      cmd_rmdir(dname);
    }
    else if (strcmp(cmd, "df") == 0)
    {
      cmd_df();
    }
    else
    {
      printf("Unknown command: %s\n", cmd);
    }
  }
}

static char *unescapeString(const char *src)
{
  if (!src)
  {
    return NULL;
  }

  char *out = malloc(strlen(src) + 1);
  if (!out)
  {
    return NULL;
  }

  int j = 0;
  for (int i = 0; src[i] != '\0'; i++)
  {
    if (src[i] == '\\')
    {
      i++;
      if (src[i] == 'n')
      {
        out[j++] = '\n';
      }
      else if (src[i] == 't')
      {
        out[j++] = '\t';
      }
      else if (src[i] == '\\')
      {
        out[j++] = '\\';
      }
      else if (src[i] == '"')
      {
        out[j++] = '"';
      }
      else
      {
        out[j++] = src[i];
      }
    }
    else
    {
      out[j++] = src[i];
    }
  }
  out[j] = '\0';
  return out;
}

static void cmd_write(const char *filename, const char *text)
{
  if (!filename || !text)
  {
    printf("Usage: write <filename> \"text\"\n");
    return;
  }

  FileNode *f = findChild(cwd, filename);
  if (!f)
  {
    printf("File not found.\n");
    return;
  }
  if (f->isDirectory)
  {
    printf("'%s' is a directory.\n", filename);
    return;
  }

  char *processedText = unescapeString(text);
  if (!processedText)
  {
    printf("Memory error while processing string.\n");
    return;
  }

  int newLen = strlen(processedText);
  int oldLen = f->contentSize;
  int totalLen = oldLen + newLen;

  int totalNeeded = (totalLen + BLOCK_SIZE - 1) / BLOCK_SIZE;
  int currentlyHave = f->numBlocks;
  int additionalNeeded = totalNeeded - currentlyHave;

  if (additionalNeeded > freeCount)
  {
    printf("Error: Not enough disk space to append.\n");
    free(processedText);
    return;
  }

  if (additionalNeeded > 0)
  {
    f->blockPointers = realloc(f->blockPointers, sizeof(int) * totalNeeded);
    for (int i = 0; i < additionalNeeded; i++)
    {
      int b = allocateBlockFromHead();
      if (b == -1)
      {
        printf("Error: Disk full.\n");
        free(processedText);
        return;
      }
      f->blockPointers[currentlyHave + i] = b;
    }
  }

  for (int i = 0; i < newLen; i++)
  {
    int globalOffset = oldLen + i;
    int blockIndex = globalOffset / BLOCK_SIZE;
    int offsetInBlock = globalOffset % BLOCK_SIZE;
    int blockNum = f->blockPointers[blockIndex];
    virtualDisk[blockNum][offsetInBlock] = processedText[i];
  }

  f->contentSize = totalLen;
  f->numBlocks = totalNeeded;

  printf("Data written successfully (size=%d bytes).\n", newLen);
  free(processedText);
}

static void cmd_read(const char *filename)
{
  if (!filename)
  {
    printf("Usage: read <filename>\n");
    return;
  }

  FileNode *f = findChild(cwd, filename);
  if (!f)
  {
    printf("Error: file not found.\n");
    return;
  }
  if (f->isDirectory)
  {
    printf("Error: '%s' is a directory.\n", filename);
    return;
  }
  if (f->numBlocks == 0)
  {
    printf("(empty file)\n");
    return;
  }

  int remaining = f->contentSize;
  for (int i = 0; i < f->numBlocks; i++)
  {
    int b = f->blockPointers[i];
    int bytes = (remaining > BLOCK_SIZE) ? BLOCK_SIZE : remaining;
    fwrite(virtualDisk[b], 1, bytes, stdout);
    remaining -= bytes;
  }
  printf("\n");
}

static void cmd_delete(const char *filename)
{
  if (!filename)
  {
    printf("Usage: delete <filename>\n");
    return;
  }

  FileNode *f = findChild(cwd, filename);
  if (!f)
  {
    printf("Error: file not found.\n");
    return;
  }
  if (f->isDirectory)
  {
    printf("Error: '%s' is a directory.\n", filename);
    return;
  }

  for (int i = 0; i < f->numBlocks; i++)
  {
    freeBlockToTail(f->blockPointers[i]);
  }

  free(f->blockPointers);
  f->blockPointers = NULL;
  f->numBlocks = 0;
  f->contentSize = 0;

  removeChild(f->parent, f);
  free(f);

  printf("File deleted successfully.\n");
}

static void cmd_rmdir(const char *dirname)
{
  if (!dirname)
  {
    printf("Usage: rmdir <dirname>\n");
    return;
  }

  FileNode *d = findChild(cwd, dirname);
  if (!d)
  {
    printf("Directory not empty. Remove files first.\n");
    return;
  }
  if (!d->isDirectory)
  {
    printf("Error: '%s' is not a directory.\n", dirname);
    return;
  }
  if (d->child)
  {
    printf("Error: Directory '%s' is not empty.\n", dirname);
    return;
  }

  removeChild(d->parent, d);
  free(d);
  printf("Directory '%s' removed successfully.\n", dirname);
}

static void cmd_df(void)
{
  int used = NUM_BLOCKS - freeCount;
  double percent = ((double)used / NUM_BLOCKS) * 100.0;
  printf("Total Blocks: %d\n", NUM_BLOCKS);
  printf("Used Blocks: %d\n", used);
  printf("Free Blocks: %d\n", freeCount);
  printf("Disk Usage: %.2f%%\n", percent);
}
