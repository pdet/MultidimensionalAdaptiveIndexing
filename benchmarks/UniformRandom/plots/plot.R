#devtools::install_github("slowkow/ggrepel")

(function(lp) {
np <- lp[!(lp %in% installed.packages()[,"Package"])]
if(length(np)) install.packages(np,repos=c("http://cran.rstudio.com/"))
x <- lapply(lp,function(x){library(x,character.only=TRUE, quietly=TRUE, warn.conflicts=FALSE)}) 
})(c("dplyr", "ggplot2", "ggthemes", "ggrepel", "data.table", "RColorBrewer"))


theme <- theme_few(base_size = 24)  +  theme(legend.position = "none", plot.title = element_text(size=16, hjust=.5, family="mono"))

dd <- fread("output.csv",fill=TRUE)
# algorithm;repetition;query_selectivity;query_zipf;query_number;query_pattern;column_size;column_pattern;bptree_elementspernode;number_of_columns;index_creation;index_lookup;scan_time;total_time

dd %>%  group_by(algorithm, query_selectivity, query_pattern,number_of_columns,repetition) %>%  mutate(query = 1:n(), cumtime=cumsum(total_time)) %>% as.data.frame() -> ddq

ddq %>% select(algorithm, query_number, query_selectivity, query_pattern, number_of_columns,index_creation,index_lookup,scan_time,total_time, cumtime) %>% group_by(algorithm, query_number, query_selectivity, query_pattern, number_of_columns) %>% summarise_all(funs(median)) -> aggr




ORDERING = c('fs'=1, 'stdavl'=2, 'stdkd'=3, 'fibpt'=4, 'fikd'=5)


aggr %>% mutate(label=recode(algorithm, fs="Scan", stdavl="Cracking (AVL)", stdkd="Cracking(KD)", fibpt="Index(B+)", fikd="Index(KD)"))%>% arrange(ORDERING[algorithm])  -> pdata2

pdata2 <- filter(pdata2, number_of_columns == 4)
pdata2 <- filter(pdata2,query_number < 50)

pdf("performance_time_cumulative.pdf", width=8, height=5)
ggplot(pdata2, aes(y=cumtime, x=query_number, color=reorder(label, ORDERING[algorithm]), group=reorder(label, ORDERING[algorithm]), shape=reorder(label, ORDERING[algorithm]))) + geom_line(size=1) + theme + ylab("Cumulative Time (s)") + xlab("Query (#)")  + theme(legend.justification = c(0, 1), legend.position = c(0, 1), legend.background = element_rect(fill = "transparent", colour = "transparent"), legend.title=element_blank())

dev.off()

