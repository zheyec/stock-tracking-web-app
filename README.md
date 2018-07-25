# Questions

## What is pneumonoultramicroscopicsilicovolcanoconiosis?

It refers to a synonym for the disease known as silicosis.

## According to its man page, what does `getrusage` do?

It returns resource usage statistics for the specified process(es).

## Per that same man page, how many members are in a variable of type `struct rusage`?

16

## Why do you think we pass `before` and `after` by reference (instead of by value) to `calculate`, even though we're not changing their contents?

This is because passing merely addresses is much more space-efficient and memory-efficient than passing rusage structures.

## Explain as precisely as possible, in a paragraph or more, how `main` goes about reading words from a file. In other words, convince us that you indeed understand how that function's `for` loop works.

The program primarily reads the file character by character, using the fgetc function. Upon reading and verifying each character, the character is appended to the current word. If the word is too long or the character is invalid, all the remaining characters before a whitespace are ignored, and this program prepares itself to read a new word.
After reading a whole word, the program goes on and checkes whether the word is in the dictionary.

## Why do you think we used `fgetc` to read each word's characters one at a time rather than use `fscanf` with a format string like `"%s"` to read whole words at a time? Put another way, what problems might arise by relying on `fscanf` alone?

This is because fscanf ignores only whitespaces and thus will include punctuations as part of the words as well.

## Why do you think we declared the parameters for `check` and `load` as `const` (which means "constant")?

This makes sure that the parameters are not altered by the functions.
