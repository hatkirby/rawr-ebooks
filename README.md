# rawr-ebooks

*I suddenly found it very hilarious.* --[@Rawr_Ebooks](https://twitter.com/Rawr_Ebooks/status/385131476141879296)

rawr-ebooks is a very good example of taking things too far. One of the assignments in the algorithms course I took was to implement an algorithm in SML that would generate nonsense statistically similar to an input corpus (basically, a plain text file with words and sentences in it). Of course, the actual point of the assignment was more focused on finding an algorithm that would do this in certain required cost bounds, but after the assignment ended, I decided that the project was too fun to let go and, combined with the recent revelation that [@Horse_Ebooks](https://twitter.com/Horse_Ebooks) was actually not a bot as widely believed, decided to augment my algorithm with the ability to post to Twitter.

rawr-ebooks actually consists of two programs: `rawr-ebooks`, which generates nonsense and posts it to a Twitter account every hour, and `rawr-gen`, which generates nonsense on command. `rawr-gen` is probably more useful for the casual, well, anybody.

Here is how one would go about compiling `rawr-gen`:

1. Clone rawr-ebooks onto your computer.

   <pre>git clone http://github.com/hatkirby/rawr-ebooks</pre>
    
2. Use autoconf and automake to generate the configure file

   <pre>autoreconf --install --force</pre>
    
3. Run configure

   <pre>./configure</pre>
    
4. Make

   <pre>make rawr-gen</pre>
    
5. Rename `config-example.yml` to `config.yml` and within it, replace `corpus.txt` with the path to your input
6. Run `rawr-gen`

   <pre>./rawr-gen</pre>

## Implementation details

I ended up rewriting the algorithm in C++ as the SML implementation did not handle randomization very well and would have been very difficult to adjust to post to Twitter. The new version has many improvements that improve the quality of the generated output, and the input corpus that I use for @Rawr_Ebooks is growing every day. As of October 6th, 2013, it is about 200,000 words long.

rawr-ebooks uses [yamlcpp](https://code.google.com/p/yaml-cpp/) to read configuration data from a file (mainly, where the input corpus is located, and the information used to connect to Twitter), and [twitcurl](https://code.google.com/p/twitcurl/) to post to Twitter.

The program is roughly divided into two stages: a preprocessing stage and a generation stage. The preprocessing stage runs once at the beginning of the program's run and generates information to ease in the generation of output. This stage runs in O(t^2) time where t is the number of tokens in the input corpus. As you can probably tell, the preprocessing stage can take a fair bit of time to run sometimes. The generation stage actually generates the output and can occur multiple times per program run (in fact it should, otherwise you aren't making good use of the time spent during the preprocessing stage!). It runs in O(n log t) time, where t is the number of tokens in the input corpus, and n is the number of words to generate, which is usually between 5 and 50. As you can see, the generation stage runs far, far more quickly than the preprocessing stage.