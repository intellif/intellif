#ifndef DB_COMMON_H_PBS6EWGU
#define DB_COMMON_H_PBS6EWGU

#define L2FEAT_LEN 180
#define PCAFEAT_LEN 180
struct db_item{
	int idx;
	int len;
	float rect[4];
	int pose;
	float feat[L2FEAT_LEN];
	float feat_pca[PCAFEAT_LEN];
};

typedef struct{
	struct db_item item;
	float score;
	float rank_score;
} mcv_face_search_result_t;

#endif /* end of include guard: DB_COMMON_H_PBS6EWGU */

