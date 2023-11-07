# RuCTF 2023 | memos

Service was written in C and given as compiled binary:

```
$ file memos
memos: ELF 64-bit LSB executable, x86-64, version 1 (GNU/Linux), statically linked, BuildID[sha1]=9ebb7293313353e770deb3a190875a1bea6a4d77, for GNU/Linux 3.2.0, not stripped
```

```
$ checksec memos
    Arch:     amd64-64-little
    RELRO:    Partial RELRO
    Stack:    Canary found
    NX:       NX enabled
    PIE:      No PIE (0x400000)
```

Source code is available at [/internal/memos/src](/internal/memos/src) directory.

## Description

The service is a meme generator, users can draw a text on a background image.

User interface (frontend) is a custom self-written HTTP server, which exposes useful handlers to control image generation. Backend uses [ImageMagick](https://imagemagick.org/) internally and calls `./magick` binary with `fork()` + `execve()` syscalls.

Users can upload their own background images, images are not protected and available for everyone. If the user wants to draw a text on the image, he creates a temporary copy of the original image and manipulates the text within the new image. The copy is assigned to the draft — temporary entity, which is exclusively controlled by user. When the meme is ready, it could be published as a public image or protected with password.

Also there is an option to create a meme from the another meme, in this case the service checks if the source meme is not protected with password. When the password exists, the generation will be aborted.

## Vulnerability

Let's look at the useful API handlers:

- Change background image

```
POST /drafts/background/<draft_uuid>

Body: <image_uuid>
```

This handler is used to change the background image of the meme. It does not check the password, so user can assign arbitrary image for the background, even if the image is protected with password.

- Draw text

```
POST /drafts/text/<draft_uuid>

Body: <text_description>
```

This handler is used to draw a text on the image and to generate new meme. This handler does perform the check for the password: if the password of the background image exists, the handler will throw an error. So the attacker can not generate meme from the protected image directly.

But there was a [TOCTOU](https://en.wikipedia.org/wiki/Time-of-check_to_time-of-use) bug in this handler:

1. check the password existence for the background image_uuid 
2. call ImageMagick and generate text labels
3. read the file pointed by image_uuid and load the background image
4. compose labels and background into the final meme

Step 1 is *check*, and step 3 is *use*. If the attacker could change the image_uuid during the step 2, the service on the step 3 would generate the meme using another image, even if the image is protected.

## Exploitation

As many of TOCTOU bugs, it could be exploitable by simple race.

ImageMagick binary runs about ~0.3 seconds, so the attacker could send two requests:

1. draw text on the public image
2. sleep ~0.1 seconds
3. change background to private image

If the race is success, the meme will be generated over the protected image.

But there is another solution, which is expected to has 100% success rate. Since the service calls `execve()` syscall after `fork()`, it's possible break `execve()` and don't replace `./memos` process. In this case ImageMagick will not be called, instead there would be two identical `./memos` processes. But the parent process will be blocked by `waitpid()` syscall, so the attacker controls the race entirely: the parent process will continue only when child process exits.

The attacker could use this trick to "freeze" the parent process and change background image in the child process. But how to break `execve()` syscall? It accepts argv parameters, and their length is limited by 128 KiB. If argv length is more than 128 KiB, `execve()` returns -1 error. The attacker controls argv, since argv contains the meme text.

Example of intended exploit: [memos.sploit.py](/sploits/memos/memos.sploit.py)

## Fix

Remove TOCTOU bug from the handler.

Since the service is compiled, it would be hard to patch the binary by hands. But there is a simpler approach: just replace `./magick` binary with a simple script, which performs a password check by itself. Then the TOCTOU bug will be eliminated.

## OCR

How to read the flag from the image?

There would be many options, the author's way is to use [tesseract](https://github.com/tesseract-ocr/tesseract) engine. It could be trained on the custom fonts using [tesstrain](https://github.com/tesseract-ocr/tesstrain) utility. Also there are many GitHub repositories with dockerized training solutions, for example: [artdevgame/tesseract-trainer](https://github.com/artdevgame/tesseract-trainer).
