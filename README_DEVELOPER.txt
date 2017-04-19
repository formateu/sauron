1. Pushing to Gerrit
    In order to push some change to Gerrit Code Review, please save,
    commit your changes and use command :

    git push origin HEAD:refs/for/<branch_name> (when it is ready for review)
    or
    git push origin HEAD:refs/drafts/<branch_name> (when it should be just saved in remote repository)

    Manually add reviewers from the other pair
    Always add project leader as reviewer.

2. Working with repository

    Please always create a separate branch for every feature and tests
    After successful Code Review it will be merged to master.
    If Code Review is negative, please apply hints from other developers
    in the same commit in your local repository using command :

    git commit --amend
    git push origin HEAD:refs...

    It will be sent as another patch set which should be reviewed too.

3. Code Review

    -1 : Something is obviously wrong with code that you review,
         please do not forget to add leave a HELPFUL comment in wrong line.
         (Also codestyle, trailing characters, ugly code, non-self-commenting code parts)

    +1 : Code seems to be okay, someone else must also approve it, it was not tested.

3a. Verified

    -1 : Program doesn't build or some tests don't pass

    +1 : Program is building and launching as expected

4. Codestyle

    Please use codestyle described here : https://google.github.io/styleguide/cppguide.html
    CODESTYLE SHOULD BE CONSISTENT IN ALL FUNCTIONS AND FILES

    Beginning of each file should start with filename, date of creation, and authors.

    If you correct some simple bug, or just a few lines, please do not add yourself as an author.
    Line should not be longer than 100 characters (usually 80)

    Some Code examples :

    for (int i = 0; i < N; ++i) {

    }

    if (...) {

    } else {

    }

    class ClassName {
    public:
        ClassName()
            : m_varName(something)
            , ...
        {}

        type publicMethodName() {}
    private:
        type m_varName;
    };
