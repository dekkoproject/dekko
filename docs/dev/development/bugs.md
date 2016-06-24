# Fixing a bug                         {#dev_bugs}

In Dekko we track all work items through bugs & blueprints. I would suggest having a read of the [Launchpad help documents](https://help.launchpad.net) to get a good idea of how these work.

## Assigning bugs

When you wish to start working on a bug you should assign the bug to yourself and mark it as "in progress".

## Assigning blueprint tasks

You can assign a bleprint task to yourself by adding your launchpad username inside square bracket and append the task with "INPROGRESS"

e.g
```bash
[my-lp-username] Implement foobar component: INPROGRESS
```
## Create local branch
The first thing you should do is create a new branch for the task. You should make sure your local & remote master repository is up to date first

```bash
$ git checkout master
$ git fetch origin/master
$ git rebase -i origin/master
// Check & save the commit message
$ git push lp master
```

Now you can create a branch. Branch names should be descriptive, usually either the bug number ```e.g fix-111920``` or a short description of what it implements ```e.g "msg-list-avatar"```

To create a new branch run:

```bash
$ git checkout -b my-new-branch-name
```
Running ```git branch``` will show you have switched to the new branch and your now ready to start working.

## Committing changes

When ready to commit some changes first you should either run `$ git status` or `$ git diff` to check you are committing the correct changes.

The changes first need staging to add a single file to the commit run

```bash
$ git add path/to/filname
```
or to add all changes at once you can just add a period

```bash
$ git add .
```

Now finally commit the changes

```bash
$ git commit
```

The commit message should display a short title of what the patch does, and a paragraph or two explaining any reasoning,issues,logic that would help the reviewer or when looking back at the commit in the future. If this is a bug fix the last line of the commit should contain ```fixes: bug #```

### example commit

```bash
Implement flippable foobar

This patch implements a flippable foobar! 
The previous rotating foobar was a bottleneck 
when multiple instances were running at the 
same time.

Using a flippable the foobars are now alot lighter.
See added benchmark tests to confirm.

fixes: bug 12398123
```

Once you have saved the commit you can push it to your remote user repository.

```bash
$ git push lp my-new-branch-name
```
## Create merge proposal

Merge proposals are created from the launchpad UI for the branch you just pushed. Make sure the target repository is lp:dekko and "master" is the target branch. Also it would be handy if you could copy/paste the commit message into the description field of the merge proposal.

Hint: use ```$ git log```

