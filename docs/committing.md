---
layout: documentation
---

# Committing

Only committers have the ability to commit your change, so make sure you
work with one during code review to get your changes committed.

If you are a committer, here are some guidelines for committing changes:

1. **Follow the format of commit messages**: we currently do not leverage
   any tooling to enforce the format of messages, so:
    a. Be clear and explicit in the commit message.
    b. Include the link to the review (this will be done automatically if
       using `support/post-reviews.py` for your own changes and
       `support/apply-review.sh` for committing changes from others).
       **Be sure to clean up the commit message when pulling in changes
       from others.**
    c. Use 72 character columns. Note that we don't always have a 50
       character or less summary because that restriction tends to cause
       people to write poorly.
2. **Keep the 'Author' intact**: `support/apply-review.sh` will handle
   this for you, but be careful when rebasing or ammending.
3. **Never ever commit a merge**: always rebase instead, as appropriate.
   Likewise, never 'force push'.
4. **Don't break the build**: we support Linux and Mac OS X, however not
   all configurations are being tested in Jenkins, so be aware of that.
   Also, pay attention to the Jenkins review bot if it flags a review as
   breaking the build. Note that if you do break the build, the fixes are
   often small and inconsequential so don't worry about going through a
   review cycle for that, just fix things (but don't take that as a license
   to "wait until the build fails to fix things").