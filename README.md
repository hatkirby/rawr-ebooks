# rawr-ebooks

*I suddenly found it very hilarious.* --[@Rawr_Ebooks](https://twitter.com/Rawr_Ebooks/status/385131476141879296)

rawr-ebooks is a very good example of taking things too far. One of the assignments in the algorithms course I took was to implement an algorithm in SML that would generate nonsense statistically similar to an input corpus (basically, a plain text file with words and sentences in it). Of course, the actual point of the assignment was more focused on finding an algorithm that would do this in certain required cost bounds, but after the assignment ended, I decided that the project was too fun to let go and, combined with the recent revelation that [@Horse_Ebooks](https://twitter.com/Horse_Ebooks) was actually not a bot as widely believed, decided to augment my algorithm with the ability to post to Twitter.

The main project in this repository is a library called `librawr` that provides an interface for generating nonsense from one or more corpuses. The repository also contains `rawr-ebooks`, the canonical Twitter bot that started it all, and `rawr-gen`, which generates the same content as `rawr-ebooks` but does not post it to Twitter.

## librawr
The interface for `librawr` can be found in `kgramstats.h`, although for readability purposes the header `rawr.h` includes the same code. The main interface is a class named `rawr`.

```
void rawr::addCorpus(std::string corpus)
```

This function takes a string and adds its content to the internal corpus. This function does not prepare `rawr` for generation; you also must call `compile`.

```
void rawr::compile(int maxK)
```

This function prepares `rawr` for text generation by tokenizing its internal corpus, analyzing word distribution, and building Markov chains. Depending on the size of your corpus, it can take a significant amount of time to run. Currently, it outputs progress information to STDOUT, although the ability to disable this will be added eventually. The parameter `maxK` controls the order of Markov chain generated; a higher number means that the generation process can keep a longer history of tokens. When `maxK` is too high, the generated output will be too similar to the provided corpus; when `maxK` is too low, the generated output will be too random. As a starting point, the canonical bot uses a `maxK` of 4.

```
void rawr::setTransformCallback(std::function<std::string(std::string, std::string)> callback)
```

With this function, you can provide a callback that `rawr` will call when generating output. When compiling its corpus, `rawr` collects words that it thinks are different forms of the same word into a distribution; usually this means words that it thinks are typos of each other. The transform callback is called after `rawr` has chosen the next word to output and after it has chosen the form of that word to output. The first parameter of the callback is the canonical form of the chosen word, while the second parameter is the chosen form. If a transform callback is provided, `rawr` will call it with this information and use the return value as the string to output.

An example: say you have a `std::set<std::string>` of words to censor in the output. Because `rawr` transforms its generated text so much, it can be difficult to censor text after generation, however you can use a transform callback to do the job as so:
```
std::set<std::string> blacklist;
rawr kgramstats;
// Initialize blacklist and kgramstats
// ...

kgramstats.setTransformCallback([&] (std::string canonical, std::string form) -> std::string {
   // Check if the next word is in the blacklist
   if (blacklist.count(canonical) == 1)
   {
      // If so, return asterisks instead
      return std::string(canonical.length(), '*');
   } else {
      // Otherwise return the form rawr chose
      return form;
   }
});
```

```
std::string rawr::randomSentence(int maxL)
```

This function actually performs text generation. It currently prints debug information to STDOUT as it works, although an option to disable this will be added eventually. The parameter `maxL` controls the termination condition. Every time a terminator token is generated (which is a token ending in some combination of periods, commas, semicolons, colons, newlines, question marks, and exclamation marks; although a single comma notably is not a terminator), `rawr` checks if `maxL` tokens have been output; if so, it ends generation. If not, there is a 25% chance that it will end generation anyway.

## rawr-ebooks and rawr-gen
`rawr-ebooks` is the canonical Twitter bot that uses `librawr` to generate text. It is hosted at [@Rawr_Ebooks](https://twitter.com/rawr_ebooks). It takes no command line arguments, and instead reads a configuration file called `config.yml`. An example of the format of this configuration file can be found in `config-example.yml`. The config file contains the filename of a corpus to use, OAuth strings used to authenticate with Twitter, and the maximum amount of time to wait between tweets.

`rawr-ebooks` uses a special word in its generation called `$name$`. It will read in a newline-delimited file in the working directory called `names.txt` and whenever the word `$name$` is chosen to be output, it is replaced by a random line from `names.txt`.

`rawr-gen` does not use `config.yml`. It instead takes the filename of the corpus to use as a command line argument.

## Implementation details

I ended up rewriting the algorithm in C++ as the SML implementation did not handle randomization very well and would have been very difficult to adjust to post to Twitter. The new version has many improvements that improve the quality of the generated output, and the input corpus that I use for @Rawr_Ebooks is growing every day. As of May 31st, 2016, it is about 440,000 words long.

`librawr` uses [aspell](http://aspell.net/) to detect typos. `rawr-ebooks` additionally uses [yaml-cpp](https://github.com/jbeder/yaml-cpp) to read configuration data from a file (mainly, where the input corpus is located, and the information used to connect to Twitter), and my own library [libtwitter++](https://github.com/hatkirby/libtwittercpp) to post to Twitter. `rawr-gen` has no external dependencies other than what `librawr` uses.
