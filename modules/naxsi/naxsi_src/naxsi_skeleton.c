/*
 * NAXSI, a web application firewall for NGINX
 * Copyright (C) 2011, Thibault 'bui' Koechlin
 *  
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here. 
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
** This files contains skeleton functions, 
** such as registred handlers. Readers already 
** aware of nginx's modules can skip most of this.
*/

#include "naxsi.h"
#include <sys/times.h>
#include <ctype.h>

    /* ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "TOP READ CONF %V %V",  */
    /* 		       &(value[0]), &(value[1]));   */
/*
** Macro used to print incorrect configuration lines
*/
#define ngx_http_dummy_line_conf_error(cf, value) do {	\
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "Naxsi-Config : Incorrect line %V %V (%s/%d)...", &(value[0]), &(value[1]), __FILE__, __LINE__); \
  } while (0)

/*
** Module's registred function/handlers.
*/
static ngx_int_t	ngx_http_dummy_access_handler(ngx_http_request_t *r);
static char		*ngx_http_dummy_read_main_conf(ngx_conf_t *cf, 
						       ngx_command_t *cmd, 
						       void *conf);
static ngx_int_t	ngx_http_dummy_init(ngx_conf_t *cf);
static char		*ngx_http_dummy_read_conf(ngx_conf_t *cf, 
						  ngx_command_t *cmd,
						  void *conf);
static void		*ngx_http_dummy_create_loc_conf(ngx_conf_t *cf);
static char		*ngx_http_dummy_merge_loc_conf(ngx_conf_t *cf, 
						       void *parent,
						       void *child);
void			*ngx_http_dummy_create_main_conf(ngx_conf_t *cf);
void			ngx_http_dummy_payload_handler(ngx_http_request_t *r);


/* command handled by the module */
static ngx_command_t  ngx_http_dummy_commands[] =  {
  /* BasicRule (in main) */
  { ngx_string(TOP_MAIN_BASIC_RULE_T),
    NGX_HTTP_MAIN_CONF|NGX_CONF_1MORE,
    ngx_http_dummy_read_main_conf,
    NGX_HTTP_MAIN_CONF_OFFSET,
    0,
    NULL },
  /* BasicRule (in loc) */
  { ngx_string(TOP_BASIC_RULE_T),
    NGX_HTTP_LOC_CONF|NGX_HTTP_LMT_CONF|NGX_CONF_1MORE,
    ngx_http_dummy_read_conf,
    NGX_HTTP_LOC_CONF_OFFSET,
    0,
    NULL },
  /* DeniedUrl */
  { ngx_string(TOP_DENIED_URL_T),
    NGX_HTTP_LOC_CONF|NGX_HTTP_LMT_CONF
    |NGX_CONF_1MORE,
    ngx_http_dummy_read_conf,
    NGX_HTTP_LOC_CONF_OFFSET,
    0,
    NULL },

  /* FlagRule */
  { ngx_string(TOP_LEARNING_FLAG_T),
    NGX_HTTP_LOC_CONF|NGX_HTTP_LMT_CONF
    |NGX_CONF_NOARGS,
    ngx_http_dummy_read_conf,
    NGX_HTTP_LOC_CONF_OFFSET,
    0,
    NULL },
  /* EnableFlag */
  { ngx_string(TOP_ENABLED_FLAG_T),
    NGX_HTTP_LOC_CONF|NGX_HTTP_LMT_CONF
    |NGX_CONF_NOARGS,
    ngx_http_dummy_read_conf,
    NGX_HTTP_LOC_CONF_OFFSET,
    0,
    NULL },
  /* DisableFlag */
  { ngx_string(TOP_DISABLED_FLAG_T),
    NGX_HTTP_LOC_CONF|NGX_HTTP_LMT_CONF
    |NGX_CONF_NOARGS,
    ngx_http_dummy_read_conf,
    NGX_HTTP_LOC_CONF_OFFSET,
    0,
    NULL },
  /* CheckRule */
  { ngx_string(TOP_CHECK_RULE_T),
    NGX_HTTP_LOC_CONF|NGX_HTTP_LMT_CONF
    |NGX_CONF_1MORE,
    ngx_http_dummy_read_conf,
    NGX_HTTP_LOC_CONF_OFFSET,
    0,
    NULL },
  ngx_null_command
};

/*
** handlers for configuration phases of the module
*/

static ngx_http_module_t ngx_http_dummy_module_ctx = {
  NULL, /* preconfiguration */
  ngx_http_dummy_init, /* postconfiguration */
  ngx_http_dummy_create_main_conf, /* create main configuration */
  NULL, /* init main configuration */
  NULL, /* create server configuration */
  NULL, /* merge server configuration */
  ngx_http_dummy_create_loc_conf, /* create location configuration */
  ngx_http_dummy_merge_loc_conf /* merge location configuration */
};


ngx_module_t ngx_http_naxsi_module = {
  NGX_MODULE_V1,
  &ngx_http_dummy_module_ctx, /* module context */
  ngx_http_dummy_commands, /* module directives */
  NGX_HTTP_MODULE, /* module type */
  NULL, /* init master */
  NULL, /* init module */
  NULL, /* init process */
  NULL, /* init thread */
  NULL, /* exit thread */
  NULL, /* exit process */
  NULL, /* exit master */
  NGX_MODULE_V1_PADDING
};

#define DEFAULT_MAX_LOC_T	10 

void *
ngx_http_dummy_create_main_conf(ngx_conf_t *cf) 
{
  ngx_http_dummy_main_conf_t	*mc;
  
  mc = ngx_pcalloc(cf->pool, sizeof(ngx_http_dummy_main_conf_t));
  if (!mc)
    return (NGX_CONF_ERROR);
  mc->locations = ngx_array_create(cf->pool, DEFAULT_MAX_LOC_T, 
				   sizeof(ngx_http_dummy_loc_conf_t *));
  if (!mc->locations)
    return (NGX_CONF_ERROR);
  return (mc);
}


/* create log conf struct */
static void *
ngx_http_dummy_create_loc_conf(ngx_conf_t *cf)
{
  ngx_http_dummy_loc_conf_t  *conf;
  
  conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_dummy_loc_conf_t));
  if (conf == NULL)
    return NULL;
  dummy_lc = conf;
  return (conf);
}

/* merge loc conf */
/* NOTE/WARNING : This function wasn't tested correctly. 
 Actually, we shouldn't merge anything, as configuration is 
 specific 'per' location ? */
static char *
ngx_http_dummy_merge_loc_conf(ngx_conf_t *cf, void *parent, 
			      void *child)
{
  ngx_http_dummy_loc_conf_t  *prev = parent;
  ngx_http_dummy_loc_conf_t  *conf = child;

  if (conf->whitelist_rules == NULL) 
    conf->whitelist_rules = prev->whitelist_rules;
  if (conf->check_rules == NULL) 
    conf->check_rules = prev->check_rules;
  if (conf->body_rules == NULL) 
    conf->body_rules = prev->body_rules;
  if (conf->header_rules == NULL) 
    conf->header_rules = prev->header_rules;
  if (conf->generic_rules == NULL) 
    conf->generic_rules = prev->generic_rules;
  return NGX_CONF_OK;
}


/*
** This function sets up handlers for ACCESS_PHASE,
** and will call the hashtable creation function
** (whitelist aggregation)
*/
static ngx_int_t 
ngx_http_dummy_init(ngx_conf_t *cf)
{
  ngx_http_handler_pt *h;
  ngx_http_core_main_conf_t *cmcf;
  ngx_http_dummy_main_conf_t *main_cf;
  ngx_http_dummy_loc_conf_t **loc_cf;
  unsigned int 				i;
  
  cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);
  main_cf = ngx_http_conf_get_module_main_conf(cf, ngx_http_naxsi_module);
  if (cmcf == NULL || 
      main_cf == NULL)
    return (NGX_ERROR);
  /* Register for access phase */
  //h = ngx_array_push(&cmcf->phases[NGX_HTTP_ACCESS_PHASE].handlers);
  h = ngx_array_push(&cmcf->phases[NGX_HTTP_REWRITE_PHASE].handlers);
  if (h == NULL) 
    return (NGX_ERROR);
  *h = ngx_http_dummy_access_handler;
  /* Go with each locations registred in the srv_conf. */
  loc_cf = main_cf->locations->elts;
  for (i = 0; i < main_cf->locations->nelts; i++) {
    if(ngx_http_dummy_create_hashtables_n(loc_cf[i], cf) != NGX_OK) {
      ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, 
			 "WhiteList Hash building failed");
      return (NGX_ERROR);
    }
  }
  return (NGX_OK);
}

/*
** my hugly configuration parsing function.
** should be rewritten, cause code is hugly and not bof proof at all
** does : top level parsing config function, 
**	  see foo_cfg_parse.c for stuff
*/
//#define readconf_debug
static char *
ngx_http_dummy_read_conf(ngx_conf_t *cf, ngx_command_t *cmd, 
			 void *conf)
{
  ngx_http_dummy_loc_conf_t	*alcf = conf, **bar;
  
  ngx_http_dummy_main_conf_t	*main_cf;
  ngx_str_t			*value;
  ngx_http_rule_t		rule, *rule_r;
  ngx_http_check_rule_t		*rule_c;
  ngx_http_custom_rule_location_t	*location;
  unsigned int	i;
  u_char			*var_end;
  
#ifdef readconf_debug
  if (cf) {
    value = cf->args->elts;
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "TOP READ CONF %V %V", 
		       &(value[0]), &(value[1]));  
  }
#endif
  if (!alcf || !cf)
    return (NGX_CONF_ERROR); 
  value = cf->args->elts;
  main_cf = ngx_http_conf_get_module_main_conf(cf, ngx_http_naxsi_module);
  if (!alcf->pushed) { 
    bar = ngx_array_push(main_cf->locations);
    if (!bar)
      return (NGX_CONF_ERROR);
    *bar = alcf;
    alcf->pushed = 1;
  }
  /* store denied URL for location */
  if (!ngx_strcmp(value[0].data, TOP_DENIED_URL_T) && value[1].len) {
    alcf->denied_url = ngx_pcalloc(cf->pool, sizeof(ngx_str_t));
    if (!alcf->denied_url)
      return (NGX_CONF_ERROR);
    alcf->denied_url->data = ngx_pcalloc(cf->pool, value[1].len+1);
    if (!alcf->denied_url->data)
      return (NGX_CONF_ERROR);
    memcpy(alcf->denied_url->data, value[1].data, value[1].len);
    alcf->denied_url->len = value[1].len;
    return (NGX_CONF_OK);
  }
  /* it's a flagrule, just a hack to enable/disable mod */
  if (!ngx_strcmp(value[0].data, TOP_ENABLED_FLAG_T)) {
    alcf->enabled = 1;
    return (NGX_CONF_OK);
  }
  /* it's a flagrule, just a hack to enable/disable mod */
  if (!ngx_strcmp(value[0].data, TOP_DISABLED_FLAG_T)) {
    alcf->force_disabled = 1;
    return (NGX_CONF_OK);
  }
  /* it's a flagrule, currently just a hack to enable/disable learning mode */
  if (!ngx_strcmp(value[0].data, TOP_LEARNING_FLAG_T)) {
    alcf->learning = 1;
    return (NGX_CONF_OK);
  }
  if (!ngx_strcmp(value[0].data, TOP_BASIC_RULE_T)) {
#ifdef readconf_debug
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "XX-TOP READ CONF %s", 
		       value[0].data);  
#endif
    memset(&rule, 0, sizeof(ngx_http_rule_t));
    if (ngx_http_dummy_cfg_parse_one_rule(cf, value, &rule, 
					   cf->args->nelts) != NGX_CONF_OK)
      {
	ngx_http_dummy_line_conf_error(cf, value);
	return (NGX_CONF_ERROR);
      }
    /* push in whitelist rules, as it have a whitelist ID array */
    if (rule.wl_id) {
#ifdef readconf_debug
      ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, 
			 "pushing rule %d in whitelist rules", 
			 rule.rule_id);  
#endif
      if (alcf->whitelist_rules == NULL) {
	alcf->whitelist_rules = ngx_array_create(cf->pool, 2,
						 sizeof(ngx_http_rule_t));
	if (alcf->whitelist_rules == NULL) {
	  return NGX_CONF_ERROR;
	}
      }
      rule_r = ngx_array_push(alcf->whitelist_rules);
      if (!rule_r) {
	return (NGX_CONF_ERROR);
      }
      memcpy(rule_r, &rule, sizeof(ngx_http_rule_t));
    }
    /* else push in appropriate ruleset */
    else {
      if (rule.br->headers) {
#ifdef readconf_debug
	ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, 
			   "pushing rule %d in header rules", 
			   rule.rule_id);  
#endif
	if (alcf->header_rules == NULL)  {
	  alcf->header_rules = ngx_array_create(cf->pool, 2,
						sizeof(ngx_http_rule_t));
	  if (alcf->header_rules == NULL) 
	    return NGX_CONF_ERROR;
	}
	rule_r = ngx_array_push(alcf->header_rules);
	if (!rule_r) return (NGX_CONF_ERROR);
	memcpy(rule_r, &rule, sizeof(ngx_http_rule_t));
      }
      /* push in body match rules (POST/PUT) */
      if (rule.br->body || rule.br->body_var) {
#ifdef readconf_debug
	ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, 
			   "pushing rule %d in body rules", rule.rule_id);  
#endif
	if (alcf->body_rules == NULL) {
	  alcf->body_rules = ngx_array_create(cf->pool, 2,
					      sizeof(ngx_http_rule_t));
	  if (alcf->body_rules == NULL) 
	    return NGX_CONF_ERROR;
	}
	rule_r = ngx_array_push(alcf->body_rules);
	if (!rule_r) return (NGX_CONF_ERROR);
	memcpy(rule_r, &rule, sizeof(ngx_http_rule_t));
      }
      /* push in generic rules, as it's matching the URI */
      if (rule.br->url) {
#ifdef readconf_debug
	ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, 
			   "pushing rule %d in generic rules", 
			   rule.rule_id);  
#endif
	if (alcf->generic_rules == NULL) {
	  alcf->generic_rules = ngx_array_create(cf->pool, 2,
						 sizeof(ngx_http_rule_t));
	  if (alcf->generic_rules == NULL) 
	    return NGX_CONF_ERROR;
	}
	rule_r = ngx_array_push(alcf->generic_rules);
	if (!rule_r) return (NGX_CONF_ERROR);
	memcpy(rule_r, &rule, sizeof(ngx_http_rule_t));
      }
      /* push in GET arg rules, but we should push in POST rules too  */
      if (rule.br->args_var || rule.br->args) {
#ifdef readconf_debug
	ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, 
			   "pushing rule %d in GET rules", rule.rule_id);  
#endif
	if (alcf->get_rules == NULL) {
	  alcf->get_rules = ngx_array_create(cf->pool, 2,
					     sizeof(ngx_http_rule_t));
	  if (alcf->get_rules == NULL) 
	    return NGX_CONF_ERROR;
	}
	rule_r = ngx_array_push(alcf->get_rules);
	if (!rule_r) return (NGX_CONF_ERROR);
	memcpy(rule_r, &rule, sizeof(ngx_http_rule_t));
      }
      /* push in custom locations. It's a rule matching a VAR_NAME or an EXACT_URI :
	 - GET_VAR, POST_VAR, URI */
      if (rule.br->custom_location) {
#ifdef readconf_debug
	ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, 
			   "pushing rule %d in custom_location rules", 
			   rule.rule_id);  
#endif
	location = rule.br->custom_locations->elts;
	for (i = 0; i < rule.br->custom_locations->nelts; i++) {
	  if (location[i].args_var) {
	    if (alcf->get_rules == NULL) {
	      alcf->get_rules = ngx_array_create(cf->pool, 2,
						 sizeof(ngx_http_rule_t));
	      if (alcf->get_rules == NULL) 
		return NGX_CONF_ERROR;
	    }
	    rule_r = ngx_array_push(alcf->get_rules);
	    if (!rule_r) return (NGX_CONF_ERROR);
	    memcpy(rule_r, &rule, sizeof(ngx_http_rule_t));
	  }
	  if (location[i].body_var) {
	    if (alcf->body_rules == NULL) {
	      alcf->body_rules = ngx_array_create(cf->pool, 2,
						  sizeof(ngx_http_rule_t));
	      if (alcf->body_rules == NULL) 
		return NGX_CONF_ERROR;
	    }
	    rule_r = ngx_array_push(alcf->body_rules);
	    if (!rule_r) return (NGX_CONF_ERROR);
	    memcpy(rule_r, &rule, sizeof(ngx_http_rule_t));
		      
	  }
	  if (location[i].headers_var) {
	    if (alcf->header_rules == NULL) {
	      alcf->header_rules = ngx_array_create(cf->pool, 2,
						    sizeof(ngx_http_rule_t));
	      if (alcf->header_rules == NULL) 
		return NGX_CONF_ERROR;
	    }
	    rule_r = ngx_array_push(alcf->header_rules);
	    if (!rule_r) return (NGX_CONF_ERROR);
	    memcpy(rule_r, &rule, sizeof(ngx_http_rule_t));
	  }
	}
      }
    }
    return (NGX_CONF_OK);
  }
  /* this should be moved in a function, plus this code is not safe. */
  else if (!ngx_strcmp(value[0].data, TOP_CHECK_RULE_T)) {
#ifdef readconf_debug
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, 
		       "pushing rule %d in check rules", rule.rule_id);  
#endif
    i = 0;
    if (!alcf->check_rules)
      alcf->check_rules = ngx_array_create(cf->pool, 2, 
					   sizeof(ngx_http_check_rule_t));
    if (!alcf->check_rules)
      return (NGX_CONF_ERROR);
    rule_c = ngx_array_push(alcf->check_rules);
    if (!rule_c) return (NGX_CONF_ERROR);
    memset(rule_c, 0, sizeof(ngx_http_check_rule_t));
    /* process the first word : score rule */
    if (value[1].data[i] == '$') {
#ifdef MDBG
	ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "XX-special score rule !");
#endif
	var_end = (u_char *) ngx_strchr((value[1].data)+i, ' ');
	if (!var_end) {
	  ngx_http_dummy_line_conf_error(cf, value);
	  return (NGX_CONF_ERROR);
	}
	rule_c->sc_tag.data = ngx_pcalloc(cf->pool, var_end - value[1].data +1);
	if (!rule_c->sc_tag.data)
	  return (NGX_CONF_ERROR);
	memcpy(rule_c->sc_tag.data, value[1].data, (var_end - value[1].data));
	i += (var_end - value[1].data) + 1;
	rule_c->sc_tag.len = (var_end - value[1].data);
      }
    else {
      ngx_http_dummy_line_conf_error(cf, value);
      return (NGX_CONF_ERROR);
    }
    // move to next word
    while (value[1].data[i] && value[1].data[i] == ' ')
      i++;
    // get the comparison type
    if (value[1].data[i] == '>' && value[1].data[i+1] == '=')
      rule_c->cmp = SUP_OR_EQUAL;
    else if (value[1].data[i] == '>' && value[1].data[i+1] != '=')
      rule_c->cmp = SUP;
    else if (value[1].data[i] == '<' && value[1].data[i+1] == '=')
      rule_c->cmp = INF_OR_EQUAL;
    else if (value[1].data[i] == '<' && value[1].data[i+1] != '=')
      rule_c->cmp = INF;
    else {
      ngx_http_dummy_line_conf_error(cf, value);
      return (NGX_CONF_ERROR);
    }
    // move to next word
    while (value[1].data[i] && !(value[1].data[i] >= '0' && 
				 value[1].data[i] <= '9') && (value[1].data[i] != '-'))
      i++;
#ifdef readconf_debug
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, 
		       "XX-special score in checkrule:%s from (%d)", 
		       value[1].data, atoi((const char *)value[1].data+i));
#endif
    // get the score
    rule_c->sc_score = atoi((const char *)(value[1].data+i));
    /* process the second word : Action rule */
    if (!ngx_strstr(value[2].data, "BLOCK"))
      rule_c->block = 1;
    else if (!ngx_strstr(value[2].data, "ALLOW"))
      rule_c->block = 1;
    else if (!ngx_strstr(value[2].data, "LOG"))
      rule_c->block = 1;
    else {
      ngx_http_dummy_line_conf_error(cf, value);
      return (NGX_CONF_ERROR);
    }
    return (NGX_CONF_OK);
  }
  ngx_http_dummy_line_conf_error(cf, value);
  return (NGX_CONF_ERROR);
}

//#define main_conf_debug
static char *
ngx_http_dummy_read_main_conf(ngx_conf_t *cf, ngx_command_t *cmd, 
			      void *conf)
{
  ngx_http_dummy_main_conf_t	*alcf = conf;
  ngx_str_t			*value;
  ngx_http_rule_t		rule, *rule_r;
  ngx_http_custom_rule_location_t	*location;
  unsigned int	i;
  if (!alcf || !cf)
    return (NGX_CONF_ERROR);  /* alloc a new rule */
  value = cf->args->elts;
  /* parse the line, fill rule struct  */
#ifdef main_conf_debug
  ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, 
		     "XX-TOP READ CONF %s", value[0].data);
#endif
  if (!ngx_strcmp(value[0].data, TOP_MAIN_BASIC_RULE_T)) {
    memset(&rule, 0, sizeof(ngx_http_rule_t));
    if (ngx_http_dummy_cfg_parse_one_rule(cf/*, alcf*/, value, &rule, 
					  cf->args->nelts) != NGX_CONF_OK) {
      ngx_http_dummy_line_conf_error(cf, value);
      return (NGX_CONF_ERROR);
    }

    if (rule.br->headers) {
#ifdef main_conf_debug
      ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, 
			 "pushing rule %d in header rules", rule.rule_id);  
#endif
      if (alcf->header_rules == NULL) {
	alcf->header_rules = ngx_array_create(cf->pool, 2,
					      sizeof(ngx_http_rule_t));
	if (alcf->header_rules == NULL) 
	  return NGX_CONF_ERROR;
      }
      rule_r = ngx_array_push(alcf->header_rules);
      if (!rule_r) return (NGX_CONF_ERROR);
      memcpy(rule_r, &rule, sizeof(ngx_http_rule_t));
    }
    /* push in body match rules (POST/PUT) */
    if (rule.br->body || rule.br->body_var) {
#ifdef main_conf_debug
      ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, 
			 "pushing rule %d in body rules", rule.rule_id);  
#endif
      if (alcf->body_rules == NULL) {
	alcf->body_rules = ngx_array_create(cf->pool, 2,
					    sizeof(ngx_http_rule_t));
	if (alcf->body_rules == NULL) 
	  return NGX_CONF_ERROR;
      }
      rule_r = ngx_array_push(alcf->body_rules);
      if (!rule_r) return (NGX_CONF_ERROR);
      memcpy(rule_r, &rule, sizeof(ngx_http_rule_t));
    }
    /* push in generic rules, as it's matching the URI */
    if (rule.br->url)	{
#ifdef main_conf_debug
      ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
			 "pushing rule %d in generic rules", rule.rule_id);  
#endif
      if (alcf->generic_rules == NULL) {
	alcf->generic_rules = ngx_array_create(cf->pool, 2,
					       sizeof(ngx_http_rule_t));
	if (alcf->generic_rules == NULL) 
	  return NGX_CONF_ERROR;
      }
      rule_r = ngx_array_push(alcf->generic_rules);
      if (!rule_r) return (NGX_CONF_ERROR);
      memcpy(rule_r, &rule, sizeof(ngx_http_rule_t));
    }
    /* push in GET arg rules, but we should push in POST rules too  */
    if (rule.br->args_var || rule.br->args) {
#ifdef main_conf_debug
      ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, 
			 "pushing rule %d in GET rules", rule.rule_id);  
#endif
      if (alcf->get_rules == NULL) {
	alcf->get_rules = ngx_array_create(cf->pool, 2,
					   sizeof(ngx_http_rule_t));
	if (alcf->get_rules == NULL) 
	  return NGX_CONF_ERROR;
      }
      rule_r = ngx_array_push(alcf->get_rules);
      if (!rule_r) return (NGX_CONF_ERROR);
      memcpy(rule_r, &rule, sizeof(ngx_http_rule_t));
    }
    /* push in custom locations. It's a rule matching a VAR_NAME or an EXACT_URI :
       - GET_VAR, POST_VAR, URI */
    if (rule.br->custom_location) {
#ifdef main_conf_debug
      ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, 
			 "pushing rule %d in custom_location rules", 
			 rule.rule_id);  
#endif
      location = rule.br->custom_locations->elts;
      for (i = 0; i < rule.br->custom_locations->nelts; i++) {
	if (location[i].args_var)	{
	  if (alcf->get_rules == NULL) {
	    alcf->get_rules = ngx_array_create(cf->pool, 2,
					       sizeof(ngx_http_rule_t));
	    if (alcf->get_rules == NULL) 
	      return NGX_CONF_ERROR;
	  }
	  rule_r = ngx_array_push(alcf->get_rules);
	  if (!rule_r) return (NGX_CONF_ERROR);
	  memcpy(rule_r, &rule, sizeof(ngx_http_rule_t));
	}
	if (location[i].body_var)	{
	  if (alcf->body_rules == NULL) {
	    alcf->body_rules = ngx_array_create(cf->pool, 2,
						sizeof(ngx_http_rule_t));
	    if (alcf->body_rules == NULL) 
	      return NGX_CONF_ERROR;
	  }
	  rule_r = ngx_array_push(alcf->body_rules);
	  if (!rule_r) return (NGX_CONF_ERROR);
	  memcpy(rule_r, &rule, sizeof(ngx_http_rule_t));
		      
	}
	if (location[i].headers_var) {
	  if (alcf->header_rules == NULL) {
	    alcf->header_rules = ngx_array_create(cf->pool, 2,
						  sizeof(ngx_http_rule_t));
	    if (alcf->header_rules == NULL) 
	      return NGX_CONF_ERROR;
	  }
	  rule_r = ngx_array_push(alcf->header_rules);
	  if (!rule_r) return (NGX_CONF_ERROR);
	  memcpy(rule_r, &rule, sizeof(ngx_http_rule_t));
	}
      }
    }
    return (NGX_CONF_OK);
  }
  ngx_http_dummy_line_conf_error(cf, value);
  return (NGX_CONF_ERROR);
}

/*
** [ENTRY POINT] does : this is the function called by nginx : 
** - Set up the context for the request
** - Check if the job is done and we're called again
** - if it's a POST/PUT request, setup hook for body dataz
** - call dummy_data_parse
** - check our context struct (with scores & stuff) against custom check rules
** - check if the request should be denied
*/
//#define mechanics_debug
static ngx_int_t ngx_http_dummy_access_handler(ngx_http_request_t *r)
{
  ngx_http_request_ctx_t	*ctx;
  ngx_int_t			rc;
  ngx_http_dummy_loc_conf_t	*cf;
  ngx_http_core_loc_conf_t  *clcf;
  struct tms		 tmsstart, tmsend;
  clock_t		 start, end;
  
  ctx = ngx_http_get_module_ctx(r, ngx_http_naxsi_module);
  cf = ngx_http_get_module_loc_conf(r, ngx_http_naxsi_module);

  clcf = ngx_http_get_module_loc_conf(r, ngx_http_core_module);
  /* ngx_log_debug(NGX_LOG_DEBUG_HTTP, r->connection->log, 0, */
  /* 		"naxsi_entry_point"); */
  
  if (ctx && ctx->over)
    return (NGX_DECLINED);
  if (ctx && ctx->wait_for_body) {
#ifdef mechanics_debug
    ngx_log_debug(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
		  "naxsi:NGX_AGAIN");
#endif
    return (NGX_DONE);
  }
  if (!cf) 
    return (NGX_ERROR);
  /* the module is not enabled here */
  if (!cf->enabled || cf->force_disabled)
    return (NGX_DECLINED);
  /* don't process internal requests. */
  if (r->internal) {
#ifdef mechanics_debug
    ngx_log_debug(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
		  "XX-DON'T PROCESS (%V)|CTX:%p|ARGS:%V|METHOD=%s|INTERNAL:%d", &(r->uri), ctx, &(r->args),
		  r->method == NGX_HTTP_POST ? "POST" : r->method == NGX_HTTP_PUT ? "PUT" : r->method == NGX_HTTP_GET ? "GET" : "UNKNOWN!!",
		  r->internal);
#endif
    return (NGX_DECLINED);
  }
#ifdef mechanics_debug
  ngx_log_debug(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
  		"XX-processing (%V)|CTX:%p|ARGS:%V|METHOD=%s|INTERNAL:%d", &(r->uri), ctx, &(r->args),
		r->method == NGX_HTTP_POST ? "POST" : r->method == NGX_HTTP_PUT ? "PUT" : r->method == NGX_HTTP_GET ? "GET" : "UNKNOWN!!",
		r->internal);
#endif
  if (!ctx) {
    ctx = ngx_pcalloc(r->pool, sizeof(ngx_http_request_ctx_t));
    /* might have been set by a previous trigger */
    if (cf->learning)	{
      clcf->post_action.data = 0; //cf->denied_url->data;
      clcf->post_action.len = 0; //cf->denied_url->len;
    }
      
    if (ctx == NULL)
      return NGX_ERROR;
    ngx_http_set_ctx(r, ctx, ngx_http_naxsi_module);
    if  ((r->method == NGX_HTTP_POST || r->method == NGX_HTTP_PUT) 
	 && !ctx->ready) {
#ifdef mechanics_debug
      ngx_log_debug(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
		    "XX-dummy : body_request : before !");
#endif
      rc = ngx_http_read_client_request_body(r, ngx_http_dummy_payload_handler);
      /* this might happen quite often, especially with big files / 
      ** low network speed. our handler is called when headers are read, 
      ** but, often, the full body request hasn't yet, so 
      ** read client request body will return ngx_again. Then we need
      ** to return ngx_done, wait for our handler to be called once 
      ** body request arrived, and let him call core_run_phases
      ** to be able to process the request.
      */
      if (rc == NGX_AGAIN) {
	ctx->wait_for_body = 1;
#ifdef mechanics_debug
	ngx_log_debug(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
		      "XX-dummy : body_request : NGX_AGAIN !");
#endif
	return (NGX_DONE);
      }
      else
	if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
	  /* this debug print should be commented, but the thing is that
	  ** according to what I read into nginx src code, it may happen
	  ** and I haven't been abble to trigger this, so I just let it 
	  ** here to know when this special case will be triggered
	  */
	  ngx_log_debug(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
			"XX-dummy : SPECIAL RESPONSE !!!!");
	  return rc;
	}
    }
    else
      ctx->ready = 1;
  }
  if (ctx && ctx->ready && !ctx->over) {
    if ((start = times(&tmsstart)) == -1)
      ngx_log_debug(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
		    "XX-dummy : Failed to get time");
    ngx_http_dummy_data_parse(ctx, r);
    cf->request_processed++;
    if ((end = times(&tmsend)) == -1)
      ngx_log_debug(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
		    "XX-dummy : Failed to get time");
    if (end - start > 0)
      ngx_log_debug(NGX_LOG_DEBUG_HTTP, r->connection->log, 0, 
		    "[MORE THAN 1MS] times : start:%l end:%l diff:%l",
		    start, end, (end-start));
    ctx->over = 1;
    if (ctx->block) {
      cf->request_blocked++;
      rc = ngx_http_output_forbidden_page(ctx, r);
      //nothing:      return (NGX_OK);
      //redirect : return (NGX_HTTP_OK);
      return (rc);
    }
  }
#ifdef mechanics_debug
  ngx_log_debug(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
		"NGX_FINISHED !");
#endif

  return (NGX_DECLINED);
}

